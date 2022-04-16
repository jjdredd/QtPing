#include <iostream>

#include <boost/system/error_code.hpp>
#include <QInputDialog>

#include "pinger_main_window.hpp"


//
// class PingerMainWindow
// 

PingerMainWindow::PingerMainWindow(QWidget *parent)
	: QMainWindow(parent) {

	m_updateDelay = 1000;

 	setupUi(this);
	
	e_HostName->setReadOnly(true);
	e_Address->setReadOnly(true);
	e_Average->setReadOnly(true);
	e_StdDev->setReadOnly(true);
	e_Min->setReadOnly(true);
	e_Max->setReadOnly(true);
	e_lost->setReadOnly(true);
	e_TTL->setReadOnly(true);

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

	connect(this, SIGNAL(ClearDisplayStats()), e_HostName, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Address, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Average, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_StdDev, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Min, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_Max, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_lost, SLOT(clear()));
	connect(this, SIGNAL(ClearDisplayStats()), e_TTL, SLOT(clear()));

	m_appCore.start();
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

	input_text = QInputDialog::getText(this, QString("Add host name or ip"),
					   QString("Host name or ip:"), QLineEdit::Normal,
					   QString(""), &ok);

	// input_text = QString("66.163.70.130");
	if (!ok || input_text.isEmpty()) { return; }

	HostListItem *li = new HostListItem();
	unsigned key = m_appCore.AddHost(input_text);
	m_appCore.SelectState(key);
	li->SetKey(key);
	li->setText(input_text);
	HostListWidget->addItem(li);

}

void PingerMainWindow::DeleteHost() {
	unsigned key = m_appCore.GetState();
	m_appCore.DeleteHost();
	// search, store, remove from list, remove from app, remove from widget, delete
	m_listItems.remove_if( [=] (HostListItem *hli) {
		if (hli->GetKey() == key) { return true; }
		return false;
	} );
	if (!m_listItems.empty()) {
		m_appCore.SelectState((*m_listItems.begin())->GetKey());
	}
	// HostListWidget->remoteItemWidget(hli);
	// delete hli;
		
}

void PingerMainWindow::ItemSelected(QListWidgetItem *litem) {
	m_appCore.SelectState(dynamic_cast<HostListItem*>(litem)->GetKey());
}

void PingerMainWindow::UpdateDisplay() {
	std::cout << "UpdateDisplay() called " << std::endl;
	if (!m_appCore.IsDataOK()) { return; }
	emit UpdateHostName(m_appCore.GetHostName());
	emit UpdateAddress(m_appCore.GetAddress());
	emit UpdateAverage(m_appCore.GetAverage());
	emit UpdateStdDev(m_appCore.GetStdDev());
	emit UpdateMin(m_appCore.GetMin());
	emit UpdateMax(m_appCore.GetMax());
	emit UpdateLost(m_appCore.GetLost());
	emit UpdateTTL(m_appCore.GetTTL());
}
