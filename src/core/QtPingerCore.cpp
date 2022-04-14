#include "QtPingerCore.hpp"


//
// class QtPingerCore
//

QtPingerCore::QtPingerCore()
	: m_pinger(&m_hosts, &m_mutex, m_ioc) {}


void QtPingerCore::StopPingerThread() {
	m_ioc.stop();
	// Qt thread will stop as soon as the run() function returns
	// that is as soon as m_ioc.run() returns
}

QString QtPingerCore::GetHostName() {
	// don't forget to lock mutex
	return QString(m_hosts[m_state].);
}

QString QtPingerCore::GetAddress() {}
QString QtPingerCore::GetAverage() {}
QString QtPingerCore::GetStdDev() {}
QString QtPingerCore::GetMin() {}
QString QtPingerCore::GetMax() {}
QString QtPingerCore::GetLost() {}
QString QtPingerCore::GetTTL() {}

