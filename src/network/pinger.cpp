#include <algorithm>

#include "pinger.hpp"

//
// HostInfo
// 

network::HostInfo::HostInfo(icmp::endpoint &host)
	: mean_latency(0), stdev_latency(0)
	, destination(host), reply_received(false) {

}

network::HostInfo::~HostInfo() {}

std::vector<ping_reply> network::HostInfo::GetReplies() {
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

void network::HostInfo::TimeSent(chrono::steady_clock::time_point &t) {
	time_last_sent = t;
	reply_received = false;
}

//
// Pinger
// 



#if 0
	destination = *resolver_.resolve(icmp::v4(), host, "").begin();

	boost::system::error_code ec;
	icmp::resolver host_resolver;
	boost::asio::ip::address ip = boost::asio::ip::make_address(host, ec);
	if (ec.failed()) {
		host_resolver();
	} else {
		destination = boost::asio::ip::endpoint(ip);
	}
#endif
