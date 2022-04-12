#include "HostContainer.hpp"

using namespace network;

//
// HostInfo
// 

HostInfo::HostInfo(icmp::endpoint &host, std::string &hs)
	: mean_latency(0), stdev_latency(0)
	, destination(host), sequence(1), reply_received(false)
	, host_string(hs) {

}

HostInfo::~HostInfo() {}

std::vector<HostInfo::ping_reply> HostInfo::GetReplies() {
	return std::move(replies);
}

void HostInfo::PushReply(ping_reply &reply) {
	replies.push_back(reply);
	reply_received = true;
}

void HostInfo::TimeSent(std::chrono::steady_clock::time_point &t) {
	time_last_sent = t;
	reply_received = false;
	sequence++;
}

icmp::endpoint HostInfo::GetDestination() const { return destination; }


//
// class HostContainer
//

HostContainer::HostContainer() {
	key = 1;
}

HostContainer::~HostContainer() {}

unsigned HostContainer::newKey() {
	return m_key++;
}

unsigned HostContainer::AddHost(std::string &host) {
	icmp::endpoint dest = *host_resolver.resolve(icmp::v4(), host, "").begin();
	HostInfo hi(dest, host);
	unsigned key = newKey();

	const std::lock_guard<std::mutex> lock(m_mutex);
	while (m_hosts.contains(key)) {
		key = newKey();
	}
	m_hosts[key] = hi;
	std::cout << "adding host: " << hi.GetDestination() << std::endl;
}

bool HostContainer::DeleteHost(unsigned key) {
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_hosts[key].erase(key) > 0;
}

void HostContainer::PushReply(unsigned key, ping_reply &reply) {
	const std::lock_guard<std::mutex> lock(m_mutex);
	m_hosts[key].PushReply(reply);
}

void TimeSent(unsigned key, std::chrono::steady_clock::time_point &tp) {
	const std::lock_guard<std::mutex> lock(m_mutex);
	m_hosts[key].TimeSent(tp);
}

icmp::endpoint HostContainer::GetDestination(unsigned key) const {
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_hosts[key].destination;
}

unsigned HostContainer::NumReplies(unsigned) const {
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_hosts[key].replies.size();
}

std::vector<HostInfo::ping_reply> HostContainer::GetReplies(unsigned) {
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_hosts[key].GetReplies();
}

unsigned HostContainer::GetSeqN(unsigned key) const {
	const std::lock_guard<std::mutex> lock(m_mutex);
	return m_hosts[key].sequence;
}
