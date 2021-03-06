#include <iostream>
#include <algorithm>

#include <QInputDialog>
#include <QMessageBox>

#include "pinger_main_window.hpp"


//
// class PingerMainWindow
// 

PingerMainWindow::PingerMainWindow(QWidget *parent)
	: QMainWindow(parent), m_lineChart(20), m_trayIcon(parent), m_hiddenState(false) {
	//                               ^^
	// Fix this magic constant!!!    ^^

	setWindowTitle("QtPinger");

	m_updateDelay = 1000;

 	setupUi(this);
	
	// set icon
	setWindowIcon(QIcon(m_trayPic));

	e_HostName->setReadOnly(true);
	e_Address->setReadOnly(true);
	e_Average->setReadOnly(true);
	e_StdDev->setReadOnly(true);
	e_Min->setReadOnly(true);
	e_Max->setReadOnly(true);
	e_lost->setReadOnly(true);
	e_TTL->setReadOnly(true);
	e_currentPing->setReadOnly(true);

	// timer signals
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(UpdateDisplay()));

	// button signals
	connect(b_AddHost, SIGNAL(clicked()), this, SLOT(AddHost()));
	connect(b_DeleteHost, SIGNAL(clicked()), this, SLOT(DeleteHost()));

	// list widget signals
	connect(HostListWidget, SIGNAL(itemClicked(QListWidgetItem *)),
		this, SLOT(ItemSelected(QListWidgetItem *)));

	// line edit slots
	connect(this, SIGNAL(UpdateHostName(const QString &)),
		e_HostName, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateAddress(const QString &)),
		e_Address, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateAverage(const QString &)),
		e_Average, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateStdDev(const QString &)),
		e_StdDev, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateMin(const QString &)),
		e_Min, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateMax(const QString &)),
		e_Max, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateLost(const QString &)),
		e_lost, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateTTL(const QString &)),
		e_TTL, SLOT(setText(const QString &)));
	connect(this, SIGNAL(UpdateCurrent(const QString &)),
		e_currentPing, SLOT(setText(const QString &)));

	connect(this, SIGNAL(ClearDisplayStats()), e_HostName, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Address, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Average, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_StdDev, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Min, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Max, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_lost, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_TTL, SLOT(clear()));

	m_lineChart.SetChartView(ChartView);
	ChartView->setRenderHint(QPainter::Antialiasing);

	initTrayIcon();

	m_timer.start(m_updateDelay);

}

PingerMainWindow::~PingerMainWindow() {
	// free widgets?
	m_appCore.StopPingerThread();
}


void PingerMainWindow::AddHost() {

	// check if host has already been added

	bool ok = true;
	QString input_text;
	unsigned key;

	input_text = QInputDialog::getText(this, QString("Add host name or ip"),
					   QString("Host name or ip:"), QLineEdit::Normal,
					   QString(""), &ok);

	if (!ok || input_text.isEmpty()) { return; }

	try {
		key = m_appCore.AddHost(input_text);
	} catch (boost::system::system_error &e) {
		QMessageBox::warning(this, "Failed to add host!",
				     "Check hostname or ip.");
		return;
	}

	HostListItem *li = new HostListItem();

	m_appCore.SelectState(key);
	li->SetKey(key);
	li->setText(input_text);
	HostListWidget->addItem(li);
	m_listItems.push_back(li);
	HostListWidget->setCurrentItem(li);

}

void PingerMainWindow::DeleteHost() {
	unsigned key = m_appCore.GetState();
	m_appCore.DeleteHost();
	// search, store, remove from list, remove from app, remove from widget, delete
	auto it = std::find_if(m_listItems.begin(), m_listItems.end(),
			  [=] (HostListItem *hli) {
				  if (hli->GetKey() == key) { return true; }
				  return false;
			  } );
	if (it == m_listItems.end()) { return; }

	HostListItem *item = *it;

	m_listItems.erase(it);
	if (!m_listItems.empty()) {
		m_appCore.SelectState((*m_listItems.begin())->GetKey());
	}
	HostListWidget->removeItemWidget(item);
	delete item;
		
}

void PingerMainWindow::ItemSelected(QListWidgetItem *litem) {
	m_appCore.SelectState(dynamic_cast<HostListItem*>(litem)->GetKey());
	UpdateDisplay();
}

void PingerMainWindow::UpdateDisplay() {
	std::cout << "UpdateDisplay() called " << std::endl;
	if (!m_appCore.UpdateData()) { return; }
	emit UpdateHostName(m_appCore.GetHostName());
	emit UpdateAddress(m_appCore.GetAddress());
	emit UpdateCurrent(m_appCore.GetCurrent());
	emit UpdateAverage(m_appCore.GetAverage());
	emit UpdateStdDev(m_appCore.GetStdDev());
	emit UpdateMin(m_appCore.GetMin());
	emit UpdateMax(m_appCore.GetMax());
	emit UpdateLost(m_appCore.GetLost());
	emit UpdateTTL(m_appCore.GetTTL());

	// update charts:
	m_lineChart.ReplacePoints(m_appCore.GetLatencyPoints());
	ChartView->repaint();

	// tooltip for tray icon
	m_trayIcon.setToolTip(e_Address->text() + ":   "
			      + e_currentPing->text() + " ms.");

	// tray message
	QString lost_hosts;
	if (m_appCore.GetDisconnectedHosts(lost_hosts)) {
		m_trayIcon.showMessage("Lost Connection to Host",
				       QString("LostConnection to host(s):")
				       + lost_hosts);
	}
}

void PingerMainWindow::initTrayIcon() {
	connect(&m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(trayClicked(QSystemTrayIcon::ActivationReason)));
	m_trayIcon.setIcon(QIcon(m_trayPic));
	m_trayIcon.show();
	return;
}

void PingerMainWindow::trayClicked(QSystemTrayIcon::ActivationReason reason) {
	switch (reason) {
	case QSystemTrayIcon::Trigger:
	case QSystemTrayIcon::DoubleClick:
		if (m_hiddenState) {
			showNormal();
		} else {
			hide();
		}
		m_hiddenState = !m_hiddenState;
		break;
	case QSystemTrayIcon::MiddleClick:

		break;
	default:
		break;
	}
}
