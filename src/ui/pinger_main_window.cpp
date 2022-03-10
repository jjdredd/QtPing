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

 	setupUi(this);
	
	e_HostName->setReadOnly(true);
	e_Address->setReadOnly(true);
	e_Average->setReadOnly(true);
	e_StdDev->setReadOnly(true);
	e_Min->setReadOnly(true);
	e_Max->setReadOnly(true);
	e_lost->setReadOnly(true);
	e_TTL->setReadOnly(true);

	connect(&m_pt, SIGNAL(setSequenceN(const QString &)),
		e_TTL, SLOT(setText(const QString &)));

	connect(b_AddHost, SIGNAL(clicked()), this, SLOT(printTextTest()));

	m_pt.start();

}

PingerMainWindow::~PingerMainWindow() {
	// free widgets?
}


void PingerMainWindow::printTextTest() {
	std::cout << "printTextTest: " << std::endl;
}

