#include <algorithm>

#include <iostream>

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
	sequence = reply.sequence;
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


static uint16_t pack_identifier(uint8_t id, integer n) {
	uint16_t idex = id;
	return (idex << 16) | (n & 0xFFFF);
}

static uint8_t unpack_number(uint16_t packet_id) {
	return packet_id & 0xFFFF;
}

static uint8_t unpack_id(uint16_t packet_id) {
	return (packet_id >> 16) & 0xFFFF;
}


//
// Pinger
//

network::Pinger::Pinger(boost::asio::io_context &io)
	: host_resolver(io), sock(io), stimer(io) {

	// compute and set identifier
	identifier = 0xA8A8;

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

		uint16_t id = pack_identifier(identifier, i);
		protocol.CreateEchoPacket(packet, id, h.sequence);
		sock.send_to(packet, h.GetDestination());
		// boost::asio::buffer(packet)
		h.TimeSent(steady_timer::clock_type::now()); // sets reply_received=false
	}
	stimer.expires_after(chrono::seconds(5));
	stimer.async_wait( [&] { this->timeOut(); } );
}

void network::Pinger::timeOut() {
	for (unsigned i = 0; i < remote_hosts.size(); i++) {
		HostInfo h = remote_hosts[i];
		HostInfo::ping_reply pr;
		// if haven't received, push reply structure set to timed out

		if (h.reply_received) {
			continue;
		}

		pr.status = HostInfo::Timeout;
		pr.sequence = ++h.sequence;
		h.PushReply(pr);
	}
	stimer.expires_after(chrono::seconds(1));
	stimer.async_wait( [&] { this->startSend(); } );
}

void network::Pinger::startReceive() {
	recvbuff.reserve(66000);
	sock.async_receive( boost::asio::buffer(recvbuff),
			    [&] (const boost::system::error_code& error, unsigned size)
			    { this->receive(size); } );
}

void receive(unsigned size) {
	ipv4_header ip_header;
	ICMP4Proto protocol;
	unsigned data_offset;
	HostInfo::ping_reply pr;

	int ip_size = ip_header.ParsePacket(recvbuff);

	std::vector<uint8_t> icmp_content(recvbuff.begin() + ip_size, recvbuff.end());
	Type icmp_type = protocol.ParseReply(icmp_content, data_offset);

	if (icmp_type != ICMP4Proto::EchoReply) {
		// not an echo reply packet,
		// the ping_reply struct will be filled out appropriately
		// in the timeout?
		std::cerr << "Received not echo: " << std::endl;
	} else {
		// this is an echo reply
		//
		// check the packet checksum?? (at least icmp)
		uint16_t packet_id = protocol.GetIdentifier();
		
		if (unpack_id(packet_id) != identifier
		    || unpack_number(packet_id) >= remote_hosts.size()) {
				// not our packet!
				// or something's wrong
		}

		HostInfo &h = remote_hosts[unpack_number(packet_id)];

		chrono::steady_clock::time_point now = chrono::steady_clock::now();
		pr.latency = now - h.time_last_sent;
		pr.status = HostInfo::Reply;
		pr.time_to_live = ip_header.time_to_live();
		pr.sequence = protocol.GetSequenceNumber();
		pr.remote_ip = ip_header.source_address();
		pr.remote_hostname = ""; // leave out the hostname for now

		h.PushReply(pr);
	}

	startReceive();
}
