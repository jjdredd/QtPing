#include <iostream>

#include <boost/system/error_code.hpp>

#include "pinger_main_window.hpp"

//
// class PingerThread
// 

PingerThread::PingerThread()
	: m_query_timer(m_ui_ioc), m_counter(0) {
}

void PingerThread::StopThread() {
	m_ui_ioc.stop();
	exit();
}

PingerThread::~PingerThread() {
	m_ui_ioc.stop();
	exit();
}

void PingerThread::run() {

	m_query_timer.expires_after(std::chrono::seconds(1));
	m_query_timer.async_wait( [&] (const boost::system::error_code& error)
	{
		this->m_counter++;
		this->text_TTL = QString::number(this->m_counter);
		std::cout << "query_timer " << this->text_TTL.toStdString() << std::endl;
		emit setSequenceN(this->text_TTL);
		m_query_timer.expires_after(std::chrono::seconds(1));
		run();
	});

	m_ui_ioc.run();

}


//
// class PingerMainWindow
// 

PingerMainWindow::PingerMainWindow(QWidget *parent)
	: QMainWindow(parent) {

	m_update_delay = 5000;

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
	connect(HostListWidget, SIGNAL(itemClicked()),
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

	
	m_timer.start(m_update_delay);

}

PingerMainWindow::~PingerMainWindow() {
	// free widgets?
}


void PingerMainWindow::AddHost() {
	std::cout << "AddHost test stub: " << std::endl;
}

