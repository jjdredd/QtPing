#include "QtPingerCore.hpp"


//
// class QtPingerCore
//

QtPingerCore::QtPingerCore()
	: m_pinger(&m_hosts, &m_mutex, m_ioc), m_key(1) {}


void QtPingerCore::StopPingerThread() {
	m_ioc.stop();
	// Qt thread will stop as soon as the run() function returns
	// that is as soon as m_ioc.run() returns
}

QString QtPingerCore::GetHostName() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString(m_hosts[m_state].GetLastReply().remote_hostname);
}

QString QtPingerCore::GetAddress() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString(m_hosts[m_state].GetLastReply().remote_ip.to_string());
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
	unsigned ans = m_hosts[m_state].GetLastReply().m_nAnswered;
	unsigned lost = m_hosts[m_state].GetLastReply().m_nLost;

	return QString(ans/lost * 100);
}

QString QtPingerCore::GetTTL() {
	std::lock_guard<std::mutex> lock(m_mutex);
	if (!m_hosts.contains(m_state)) { return QString(); }
	return QString(m_hosts[m_state].GetLastReply().time_to_live);
}

void QtPingerCore::SelectState(unsigned state) { m_state = state; }

unsigned QtPingerCore::GetState() const { return m_state; }

unsigned QtPingerCore::AddHost(QString &hostname) {
	std::string host_string(hostname.c_str());
	while (m_hosts.contains(m_key)) { m_key++; }
	m_pinger.AddHost(&m_hosts, host_string, m_key);
	m_state = m_key;
}

void QtPingerCore::DeleteHost() {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_hosts.erase(m_state);
}

void QtPingerCore::run() override {
	m_ioc.run();
}
