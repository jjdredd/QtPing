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
	// Qt emits a signal when thread finishes
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
	return QString::number(m_hostStats.at(m_state).GetPing(), 'f', 2);
}

QString QtPingerCore::GetAverage() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetMean(), 'f', 2);
}

QString QtPingerCore::GetStdDev() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetStdDev(), 'f', 2);
}

QString QtPingerCore::GetMin() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetMin(), 'f', 2);
}

QString QtPingerCore::GetMax() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetMax(), 'f', 2);
}

QString QtPingerCore::GetLost() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetLostPercent(), 'f', 2);
}

QString QtPingerCore::GetTTL() {
	if (!m_hostStats.contains(m_state)) { return QString(); }
	return QString::number(m_hostStats.at(m_state).GetTTL());
}

QVector<QPointF> QtPingerCore::GetLatencyPoints() const {
	return latencyPoints;
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

	// only for the current state
	if (!m_hosts.contains(m_state)) { return m_isDataOk; }
	const auto replies = m_hosts.at(m_state).GetAllReplies();
	latencyPoints.clear();
	latencyPoints.reserve(20); // m_nChartPoints
	// save points for charts
	for (const auto &r: replies) {
		// check if reply is actually valid!!
		float y_val = r.status == network::HostInfo::Reply ? r.latency.count() : 0;
		latencyPoints.push_back(QPointF(r.sequence, y_val));
	}
	return m_isDataOk;
}


unsigned QtPingerCore::AddHost(QString &hostname) {
	std::string host_string(hostname.toStdString());
	while (m_hosts.contains(m_key)) { m_key++; }
	m_pinger.AddHost(host_string, m_key);
	m_hostStats.insert_or_assign(m_key, HostDataStatistics());
	if (!isRunning()) {
		start();
	} else {
		m_pinger.StartHostPing(m_key, !isRunning());
	}
	return m_state = m_key;
}

void QtPingerCore::DeleteHost() {
	std::lock_guard<std::mutex> lock(m_mutex);
	m_hosts.erase(m_state);
	m_hostStats.erase(m_state);
}

void QtPingerCore::run() {
	m_pinger.StartPing();
	if (m_ioc.stopped()) {
		m_ioc.restart();
		m_ioc.run();
	} else {
		m_ioc.run();
	}
}


