#include <algorithm>

#include "pinger.hpp"
#include "icmp4.hpp"

//
// HostInfo
// 

network::HostInfo::HostInfo(icmp::endpoint &host, std::string &hs)
	: mean_latency(0), stdev_latency(0)
	, destination(host), sequence(0), reply_received(false)
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

icmp::endpoint network::HostInfo::GetDestination() const { return destination; }

bool network::HostInfo::ReplyReceived() const {
	return reply_received;
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
	for (unsigned i = 0; i < remote_hosts.size(); i++) {
		HostInfo h = remote_hosts[i];

		ICMP4Proto protocol;
		std::vector<uint8_t> packet;

		uint16_t id = (identifier << 16) | (i & 0xFFFF);
		protocol.CreateEchoPacket(packet, id, h.sequence);
		sock.send_to(packet, h.GetDestination());
		// boost::asio::buffer(packet)
		h.TimeSent(steady_timer::clock_type::now());
	}
	stimer.expires_after(chrono::seconds(5));
	stimer.async_wait( [&] { this->timeOut(); } );
}

void network::Pinger::timeOut() {
	for (unsigned i = 0; i < remote_hosts.size(); i++) {
		HostInfo h = remote_hosts[i];
		HostInfo::ping_reply pr;
		// if haven't received, push reply structure set to timed out

		if (h.ReplyReceived()) {
			continue;
		}

		pr.status = HostInfo::Timeout;
		// get and set sequence number
		// make hostinfo friend class?
	}
	stimer.expires_after(chrono::seconds(1));
	stimer.async_wait( [&] { this->startSend(); } );
}

void network::Pinger::startReceive() {

}

void receive() {}
