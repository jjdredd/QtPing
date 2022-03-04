#include "pinger_main_window.hpp"

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
}

PingerMainWindow::~PingerMainWindow() {}	// delete all the widgets?
