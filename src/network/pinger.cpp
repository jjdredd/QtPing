#include <algorithm>

#include <iostream>

#include "pinger.hpp"
#include "icmp4.hpp"

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


static uint16_t pack_identifier(uint8_t id, unsigned n) {
	uint16_t idex = id;
	return (idex << 8) | (n & 0xFF);
}

static uint8_t unpack_number(uint16_t packet_id) {
	return packet_id & 0xFF;
}

static uint8_t unpack_id(uint16_t packet_id) {
	return (packet_id >> 8) & 0xFF;
}


//
// Pinger
//

network::Pinger::Pinger(boost::asio::io_context &io)
	: host_resolver(io), sock(io, icmp::v4()), stimer(io) {

	// compute and set identifier
	identifier = 0xA8;
	requestBody.resize(s_dataBufferSize, 0xFF);

	bufsz = 66000;
	recvbuff.resize(bufsz, 0);

	startSend();
	startReceive();
}

network::Pinger::~Pinger() {}

icmp::endpoint network::Pinger::resolveHostOrIP(std::string &host) {

	icmp::endpoint dest;

	boost::system::error_code ec;
	dest = *host_resolver.resolve(icmp::v4(), host, "").begin();
	// boost::asio::ip::address ip = boost::asio::ip::make_address(host, ec);
	// if (ec.failed()) {
	// 	dest = *host_resolver.resolve(icmp::v4(), host, "").begin();
	// 	std::cout << "resolving host name" << std::endl;
	// } else {
	// 	std::cout << "using an IP addres" << std::endl;
	// 	dest = boost::asio::ip::icmp::endpoint(ip, 10);
	// }

	return dest;
}

void network::Pinger::AddHost(std::string &host) {
	icmp::endpoint dest = resolveHostOrIP(host);
	HostInfo hi(dest, host);
	remote_hosts.push_back(hi);
	std::cout << "adding host: " << hi.GetDestination() << std::endl;
}

// add an option to remove hosts too

void network::Pinger::startSend() {
	for (unsigned i = 0; i < remote_hosts.size(); i++) {
		HostInfo &h = remote_hosts[i];
		

		ICMP4Proto protocol;
		uint16_t id = pack_identifier(identifier, i + 1);

		requestBody.resize(s_dataBufferSize, 0xFF);
		std::fill(requestBody.begin(), requestBody.end(), 0xFF);

		std::vector<uint8_t> packet = protocol.CreateEchoPacket(requestBody,
									id, h.sequence);

		std::cerr << ">>>> sending " << packet.size() << " bytes, Sequence "
			  << h.sequence << ", to: "
			  << h.GetDestination() << std::endl;

		// std::cout << '[';
		// for (const uint8_t b : packet) {
		// 	std::cout << "0x" << std::hex << (unsigned) b << std::dec << ", ";
		// }
		// std::cout << ']' << std::endl;

		sock.send_to(boost::asio::buffer(packet), h.GetDestination());
		std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
		h.TimeSent(now); // sets reply_received=false
	}
	stimer.expires_after(std::chrono::seconds(5));
	stimer.async_wait( [&] (const boost::system::error_code& error)
	{
		if (error) {
			std::cerr << "start send error " << error.message() << std::endl;
		}
		this->timeOut();
	});
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
		pr.sequence = ++h.sequence; // why "++" ?
		h.PushReply(pr);
	}
	stimer.expires_after(std::chrono::seconds(1));
	stimer.async_wait( [&] (const boost::system::error_code& error)
	{
		if (error) {
			std::cerr << "timeout error " << error.message() << std::endl;
		}
		this->startSend();
	});
}

void network::Pinger::startReceive() {
	sock.async_receive( boost::asio::buffer(recvbuff),
			    [&] (const boost::system::error_code& error, std::size_t size)
			    {
				    if(error) {
					    std::cerr << error.message() << std::endl;
					    return;
				    }
				    this->receive(size);
			    });
}

void network::Pinger::receive(std::size_t size) {
	network::ipv4_header ip_header;
	network::ICMP4Proto protocol;
	unsigned data_offset;
	network::HostInfo::ping_reply pr;

	std::cerr << "<<<< Received " << size << " bytes" << std::endl;

	if (size == 0) {
		startReceive();
		return;
	}

	int ip_size = ip_header.ParsePacket(recvbuff);

	std::vector<uint8_t> icmp_content(recvbuff.begin() + ip_size, recvbuff.end());
	ICMP4Proto::Type icmp_type = protocol.ParseReply(icmp_content, data_offset);

	if (icmp_type != ICMP4Proto::EchoReply) {
		// not an echo reply packet,
		// the ping_reply struct will be filled out appropriately
		// in the timeout?
		std::cerr << "Received not echo: " << icmp_type << std::endl;
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

		int host_index = unpack_number(packet_id) - 1;

		if (host_index >= 0 && host_index < remote_hosts.size()) {
		
			HostInfo &h = remote_hosts[host_index];
			icmp::endpoint remote_ep;

			std::chrono::steady_clock::time_point now
				= std::chrono::steady_clock::now();

			pr.latency = now - h.time_last_sent;
			pr.status = HostInfo::Reply;
			pr.time_to_live = ip_header.time_to_live();
			pr.sequence = protocol.GetSequenceNumber();
			pr.remote_ip = ip_header.source_address();

			// reverse resolve remote hostname (add error check?)
			remote_ep.address(pr.remote_ip);

			pr.remote_hostname
				= host_resolver.resolve(remote_ep).begin()->host_name();

			// testing
			std::cout << "\t Sequence " << pr.sequence
				  << ", Latency "
				  << pr.latency.count()
				  << " milliseconds, ttl " << pr.time_to_live
				  << " from " << pr.remote_hostname
				  << " (" << pr.remote_ip.to_string() << ')' << std::endl;
			// % loss

			h.PushReply(pr);
		} 
	}

	startReceive();
}
