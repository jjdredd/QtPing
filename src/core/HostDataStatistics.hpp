#pragma once

#include <vector>
#include <QString>

#include "statistics.hpp"
#include "pinger.hpp"

static const unsigned MaxLostToReport = 10;

using namespace network;

class HostDataStatistics {

	// 1. move lost % calculation here as well
	// 2. make pinger/hostinfo return a batch of last replies
	// which haven't been accessed before (by keeping track
	// of new replies since the last access)

public:
	HostDataStatistics()
		: m_currentPing(0), m_min(10000), m_max(0)
		, m_timeToLive(0), m_nAnswered(0), m_nLost(0)
		, m_nlastLost(0), m_nMaxLost(MaxLostToReport), m_stdDev(0) {}

	~HostDataStatistics() {}

	// add a reply and compute statistics
	void ReplyUpdate(const std::vector<HostInfo::ping_reply> &rv) {
		for (const auto &pr : rv) { ReplyUpdateSingle(pr); }
	}

	void ReplyUpdateSingle(const HostInfo::ping_reply &pr) {
		if (pr.status != HostInfo::Reply) {
			m_nLost++;
			m_nlastLost++;
			return;
		}

		m_timeToLive = pr.time_to_live;
		m_currentPing = pr.latency.count();
		m_sequence = pr.sequence;
		m_ipAddress = QString::fromStdString(pr.remote_ip.to_string());
		m_hostName = QString::fromStdString(pr.remote_hostname);

		m_stdDev.ConsumeDataPoint(pr.latency.count());
		m_nAnswered++;

		if (m_currentPing < m_min) { m_min = m_currentPing; }
		if (m_currentPing > m_max) { m_max = m_currentPing; }
		m_nlastLost = 0;
	}

	// setters
	void SetHostName(QString &hn) { m_hostName = hn; }
	void SetIpAddress(QString &ipa) { m_ipAddress = ipa; }
	void SetMaxLost(unsigned n) { m_nMaxLost = n; }

	// getters
	QString GetHostName() const { return m_hostName; }
	QString GetIpAddress() const { return m_ipAddress; }

	float GetPing() const { return m_currentPing; }
	float GetMin() const { return m_min; }
	float GetMax() const { return m_max; }
	float GetLostPercent() const {
		if (m_nAnswered == 0) { return 0; }
		return ((float) m_nLost) / m_nAnswered * 100;
	}

	float GetMean() const { return m_stdDev.CurrentMean(); }
	float GetStdDev() const { return m_stdDev.CurrentValue(); }

	unsigned GetTTL() const { return m_timeToLive; }

	bool ReportDisconnect() const { return m_nlastLost >= m_nMaxLost; }
	
private:
	QString m_hostName, m_ipAddress;
	float m_currentPing, m_min, m_max;
	unsigned m_timeToLive, m_sequence, m_nAnswered, m_nLost;
	unsigned m_nlastLost, m_nMaxLost;

	StatAggregatorWelfordStD<float> m_stdDev;

};
