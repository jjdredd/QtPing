#include "QtPingerCore.hpp"


//
// class QtPingerCore
//

QtPingerCore::QtPingerCore()
	: m_pinger(&m_hosts, &m_mutex, m_ioc), m_key(1) {}

QtPingerCore::~QtPingerCore() {
	StopPingerThread();
}


void QtPingerCore::StopPingerThread() {
	m_ioc.stop();
	// Qt thread will stop as soon as the run() function returns
	// that is as soon as m_ioc.run() returns
}

QString QtPingerCore::GetHostName() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString(m_hosts.at(m_state).GetLastReply().remote_hostname.c_str());
}

QString QtPingerCore::GetAddress() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString(m_hosts.at(m_state).GetLastReply().remote_ip.to_string().c_str());
}

QString QtPingerCore::GetAverage() {
	return QString();
}

QString QtPingerCore::GetStdDev() {
	return QString();
}

QString QtPingerCore::GetMin() {
	return QString();
}

QString QtPingerCore::GetMax() {
	return QString();
}

QString QtPingerCore::GetLost() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString::number(m_hosts.at(m_state).GetLostPercent(), 'f');
}

QString QtPingerCore::GetTTL() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString(m_hosts.at(m_state).GetLastReply().time_to_live);
}

void QtPingerCore::SelectState(unsigned state) { m_state = state; }

unsigned QtPingerCore::GetState() const { return m_state; }

unsigned QtPingerCore::AddHost(QString &hostname) {
	std::string host_string(hostname.toStdString());
	while (m_hosts.contains(m_key)) { m_key++; }
	m_pinger.AddHost(&m_hosts, host_string, m_key);
	return m_state = m_key;
}

void QtPingerCore::DeleteHost() {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_hosts.erase(m_state);
}

void QtPingerCore::run() {
	m_ioc.run();
}


