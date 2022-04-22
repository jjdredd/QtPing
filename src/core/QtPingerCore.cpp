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
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return m_hostStats.at(m_state).GetHostName();
}

QString QtPingerCore::GetAddress() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return m_hostStats.at(m_state).GetIpAddress();
}

QString QtPingerCore::GetCurrent() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetPing(), 'f');
}

QString QtPingerCore::GetAverage() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetMean(), 'f');
}

QString QtPingerCore::GetStdDev() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetStdDev(), 'f');
}

QString QtPingerCore::GetMin() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetMin(), 'f');
}

QString QtPingerCore::GetMax() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetMax(), 'f');
}

QString QtPingerCore::GetLost() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetLostPercent(), 'f');
}

QString QtPingerCore::GetTTL() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetTTL());
}


// Check if the state is existing
void QtPingerCore::SelectState(unsigned state) { m_state = state; }

unsigned QtPingerCore::GetState() const { return m_state; }

bool QtPingerCore::IsDataOK() { return m_isDataOk; }

bool QtPingerCore::UpdateData() {
	std::lock_guard<std::mutex> lock(m_mutex);
	for (auto it = m_hosts.begin(); it != m_hosts.end(); it++) {
		std::vector<network::HostInfo::ping_reply> rv;
		rv = it->second.GetNewReplies();
		if (!m_isDataOk) { m_isDataOk = !rv.empty(); }
		m_hostStats.at(it->first).ReplyUpdate(rv);
	}
	return m_isDataOk;
}


unsigned QtPingerCore::AddHost(QString &hostname) {
	std::string host_string(hostname.toStdString());
	while (m_hosts.contains(m_key)) { m_key++; }
	m_pinger.AddHost(host_string, m_key);
	m_hostStats.insert_or_assign(m_key, HostDataStatistics());
	return m_state = m_key;
}

void QtPingerCore::DeleteHost() {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_hosts.erase(m_state);
	m_hostStats.erase(m_state);
}

void QtPingerCore::run() {
	m_ioc.run();
}


