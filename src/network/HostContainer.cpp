#include "HostContainer.hpp"


//
// HostInfo
// 

network::HostInfo::HostInfo(icmp::endpoint &host, std::string &hs)
	: mean_latency(0), stdev_latency(0)
	, destination(host), sequence(1), reply_received(false)
	, host_string(hs) {

}

network::HostInfo::~HostInfo() {}

std::vector<network::HostInfo::ping_reply> network::HostInfo::GetReplies() {
	return std::move(replies);
}

void network::HostInfo::PushReply(ping_reply &reply) {
	computeStats(reply);
	replies.push_back(reply);
	reply_received = true;
}

void network::HostInfo::computeStats(ping_reply &reply) {
	// compute avg latency and stdev
	// also compute time and time difference
}

void network::HostInfo::TimeSent(std::chrono::steady_clock::time_point &t) {
	time_last_sent = t;
	reply_received = false;
	sequence++;
}

icmp::endpoint network::HostInfo::GetDestination() const { return destination; }

