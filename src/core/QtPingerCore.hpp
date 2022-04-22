#pragma once

#include <QString>
#include <QThread>

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>

#include "pinger.hpp"
#include "HostDataStatistics.hpp"


// 
// class QtPingerCore
// core of the application

class QtPingerCore : public QThread {

public:
	QtPingerCore();
	virtual ~QtPingerCore();

	void StopPingerThread();

	QString GetHostName();
	QString GetAddress();
	QString GetCurrent();
	QString GetAverage();
	QString GetStdDev();
	QString GetMin();
	QString GetMax();
	QString GetLost();
	QString GetTTL();
	// etc
	// add more update functions for each field
	// to connect to each ui element

	void SelectState(unsigned); // select current state using a key
	unsigned GetState() const;

	bool IsDataOK();
	bool UpdateData();

	unsigned AddHost(QString &); // connect to main window class
	void DeleteHost();	 // connect to main window class

private:

	void run() override;	// must call start to call this

	// void updatePingData();
	// void handleOverflow();

	std::unordered_map<unsigned, HostDataStatistics> m_hostStats;
	std::unordered_map<unsigned, network::HostInfo> m_hosts;
	std::mutex m_mutex;
	unsigned m_state;
	boost::asio::io_context m_ioc;
	network::Pinger m_pinger;
	bool m_isDataOk;
	// since m_pinger will be running in another thread
	// where we need to call m_ioc.run (see test and asio docs)

	unsigned m_key;
	// move all sorts of statistical analysis here
	// move this to another folder?
};

// HostData class with stats
