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
	std::lock_guard<std::mutex> lock(m_mutex);
	return QString(m_hosts[m_state].GetLastReply().remote_hostname);
}

QString QtPingerCore::GetAddress() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return QString(m_hosts[m_state].GetLastReply().remote_ip.to_string());
}

QString QtPingerCore::GetAverage() {
	std::lock_guard<std::mutex> lock(m_mutex);
	return QString(m_hosts[m_state].GetLastReply().remote_ip.to_string());
}

QString QtPingerCore::GetStdDev() {
}

QString QtPingerCore::GetMin() {

}

QString QtPingerCore::GetMax() {

}

QString QtPingerCore::GetLost() {

}

QString QtPingerCore::GetTTL() {

}

