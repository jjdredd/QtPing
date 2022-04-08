#pragma once

#include <QMutex>
#include <QString>

#include <iostream>
#include <vector>
#include <string>

#include "pinger.hpp"


// 
// class QtPingerCore
// core of the application

class QtPingerCore : public QThread {
	Q_OBJECT;

public:
	QtPingerCore();

	void StopPingerThread();

	QString GetHostName();
	QString GetAddress();
	QString GetAverage();
	QString GetStdDev();
	QString GetMin();
	QString GetMax();
	QString GetLost();
	QString GetTTL();
	// etc
	// add more update functions for each field
	// to connect to each ui element

signals:

public slots:
	void AddHost(QString &); // connect to main window class
	void DeleteHost();	 // connect to main window class

private:

	void run() override;	// must call start to call this

	void updatePingData();
	void handleOverflow();

	std::vector<network::HostInfo> m_hostInfo;
	unsigned m_state;
	boost::asio::io_context m_ioc;
	network::Pinger m_pinger;
	// since m_pinger will be running in another thread
	// where we need to call m_ioc.run (see test and asio docs)

	// move all sorts of statistical analysis here
	// move this to another folder?
};

// HostData class with stats
