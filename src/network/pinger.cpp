#include <algorithm>

#include "pinger.hpp"
#include "icmp4.hpp"

//
// HostInfo
// 

network::HostInfo::HostInfo(icmp::endpoint &host, std::string &hs)
	: mean_latency(0), stdev_latency(0)
	, destination(host), reply_received(false)
	, host_string(name) {

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

network::Pinger::Pinger(boost::asio::io_context &io)
	: host_resolver(io), sock(io), stimer(io) {

	startSend();
	startReceive();
}

network::Pinger::~Pinger() {}

icmp::endpoint network::Pinger::resolveHostOrIP(std::string &host) {

	// destination = *resolver_.resolve(icmp::v4(), host, "").begin();

	icmp::endpoint dest;

	boost::system::error_code ec;
	boost::asio::ip::address ip = boost::asio::ip::make_address(host, ec);
	if (ec.failed()) {
		dest = *resolver_.resolve(icmp::v4(), host, "").begin();
	} else {
		dest = boost::asio::ip::endpoint(ip);
	}

	return dest;
}

void network::Pinger::AddHost(std::string &host) {
	icmp::endpoint dest = resolveHostOrIP(host);
	HostInfo hi(dest, host);
	remote_hosts.push_back(hi);
}

// add an option to remove hosts too

void network::Pinger::startSend() {
	for (const HostInfo &h : remote_hosts) {
		ICMP4Proto protocol;
		// need to create packet
		sock.send_to();
		// ctime = steady_timer::clock_type::now();
	}
	stimer.
}

void network::Pinger::timeOut() {

}

void network::Pinger::startReceive() {

}

void receive() {}
