#include <iostream>

#include <boost/system/error_code.hpp>

#include "pinger_main_window.hpp"

PingerMainWindow::PingerMainWindow(QWidget *parent)
	: QMainWindow(parent), ui_ioc(), query_timer(ui_ioc) {

 	setupUi(this);
	
	e_HostName->setReadOnly(true);
	e_Address->setReadOnly(true);
	e_Average->setReadOnly(true);
	e_StdDev->setReadOnly(true);
	e_Min->setReadOnly(true);
	e_Max->setReadOnly(true);
	e_lost->setReadOnly(true);
	e_TTL->setReadOnly(true);

	connect(this, SIGNAL(setSequenceN(const QString &)),
		e_TTL, SLOT(setText(const QString &)));

	counter = 1;

	query_timer.expires_after(std::chrono::seconds(1));
	query_timer.async_wait( [&] (const boost::system::error_code& error)
	{
		this->counter++;
		std::cout << "query_timer" << std::endl;
		emit setSequenceN(QString(this->counter));
	});

}

PingerMainWindow::~PingerMainWindow() {}	// delete all the widgets?
