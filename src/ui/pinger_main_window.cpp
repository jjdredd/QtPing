#include <iostream>

#include <boost/system/error_code.hpp>

#include "pinger_main_window.hpp"


//
// class PingerMainWindow
// 

PingerMainWindow::PingerMainWindow(QWidget *parent)
	: QMainWindow(parent) {

	m_updateDelay = 5000;

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
	connect(&m_timer, SIGNAL(QTimer::timeout), this, SLOT(UpdateDisplay()));

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

	QString remote_host_1("66.163.70.130");
	unsigned key = m_appCore.AddHost(remote_host_1);
	m_appCore.SelectState(key);
}

void PingerMainWindow::DeleteHost() {
	unsigned key = m_appCore.GetState();
	m_appCore.DeleteHost();
	m_listItems.remove_if( [=] (HostListItem *hli) {
		if (hli->GetKey() == key) { return true; }
		return false;
	} );
	if (!m_listItems.empty()) {
		m_appCore.SelectState((*m_listItems.begin())->GetKey());
	}
}

void PingerMainWindow::ItemSelected(QListWidgetItem *litem) {
	m_appCore.SelectState(dynamic_cast<HostListItem*>(litem)->GetKey());
}

void PingerMainWindow::UpdateDisplay() {
	UpdateHostName(m_appCore.GetHostName());
	UpdateAddress(m_appCore.GetAddress());
	UpdateAverage(m_appCore.GetAverage());
	UpdateStdDev(m_appCore.GetStdDev());
	UpdateMin(m_appCore.GetMin());
	UpdateMax(m_appCore.GetMax());
	UpdateLost(m_appCore.GetLost());
	UpdateTTL(m_appCore.GetTTL());
}
