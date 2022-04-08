#pragma once

#include <QMutex>

#include <iostream>
#include <vector>
#include <string>

#include "pinger.hpp"

class QtPingerCore : public QThread {
	Q_OBJECT;

public:
	QtPingerCore();

signals:
	void UpdateTTL();
	void UpdateAvg();
	void UpdateStdDev();
	// etc
	// add more update functions for each field
	// to connect to each ui element

public slots:
	void AddHost(QString &); // connect to main window class
	void DeleteHost();	 // connect to main window class

private:

	void run() override;

	void updatePingData();
	void handleOverflow();

	std::vector<network::HostInfo> m_hostInfo;
	unsigned m_state;
	boost::asio::io_context m_ioc;
	network::Pinger m_pinger;
	QMutex m_mutex;
	// since m_pinger will be running in another thread
	// where we need to call m_ioc.run (see test and asio docs)

	// move all sorts of statistical analysis here
	// move this to another folder?
};

// HostData class with stats
