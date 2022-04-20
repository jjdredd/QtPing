#pragma once

#include <vector>
#include <QString>

#include "statistics.hpp"
#include "pinger.hpp"

class HostDataStatistics {

	// 1. move lost % calculation here as well
	// 2. make pinger/hostinfo return a batch of last replies
	// which haven't been accessed before (by keeping track
	// of new replies since the last access)

public:
	HostDataStatistics()
		: m_currentPing(0), m_min(-1), m_max(0)
		, m_lostPercent(0), m_timeToLive(0) {}

	~HostDataStatistics();

	// add a reply and compute statistics
	void ReplyUpdate(std::vector<ping_reply> &rv);
	void ReplyUpdateSingle(ping_reply &pr);

	// setters
	void SetHostName(QString &hn) { m_hostName = hn; }
	void SetIpAddress(QString &ipa) { m_ipAddress = ipa; }

	// getters
	QString GetHostName() const { return m_hostName; }
	QString GetIpAddress() const { return m_ipAddress; }

	float GetPing() const { return m_currentPing; }
	float GetMin() const { return m_min; }
	float GetMax() const { return m_max; }
	float GetLostPercent() const { return m_lostPercent; }

	float GetMean() const { return m_stdDev.CurrentMean(); }
	float GetStdDev() const { return m_stdDev.CurrentValue(); }

	unsigned GetTTL() const { return m_timeToLive; }
	
private:
	QString m_hostName, m_ipAddress;
	float m_currentPing, m_min, m_max, m_lostPercent;
	unsigned m_timeToLive;

	StatAggregatorWelfordStD<float> m_stdDev;

};
