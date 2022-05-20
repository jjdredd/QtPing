#include <algorithm>

#include <iostream>

#include "pinger.hpp"
#include "icmp4.hpp"

using namespace network;

//
// HostInfo
// 

network::HostInfo::HostInfo(boost::asio::steady_timer *timer,
			    icmp::endpoint &host, std::string &hs)
	: destination(host), sequence(1), reply_received(false)
	, host_string(hs), m_stimer(timer), replies(100), m_newReplies(0) {}
//                                               ^^^
//                                 fix this magic constant


network::HostInfo::~HostInfo() {
	if (m_stimer) {
		m_stimer->cancel();
	}
}

bool network::HostInfo::IsRepliesEmpty() const {
	return replies.empty();
}

network::HostInfo::ping_reply network::HostInfo::GetLastReply() const {
	return replies.back();
}

std::vector<network::HostInfo::ping_reply> network::HostInfo::GetAllReplies() const {
	return std::vector<ping_reply> (replies.begin(), replies.end());
}

std::vector<network::HostInfo::ping_reply> network::HostInfo::GetNewReplies() {
	unsigned new_replies = m_newReplies;
	m_newReplies = 0;
	return std::vector<ping_reply> (replies.end() - new_replies, replies.end());
}

void network::HostInfo::PushReply(ping_reply &reply) {
	replies.push_back(reply);
	reply_received = true;
	m_newReplies++;
}

void network::HostInfo::TimeSent(std::chrono::steady_clock::time_point &t) {
	time_last_sent = t;
	reply_received = false;
	sequence++;
}

icmp::endpoint network::HostInfo::GetDestination() const { return destination; }


//
// Pinger
//

network::Pinger::Pinger(std::unordered_map<unsigned, HostInfo> *him, std::mutex *mtx,
			boost::asio::io_context &io)
	: host_resolver(io), sock(io, icmp::v4())
	, m_pHosts(him), m_pMutex(mtx), m_ioc(io) {

	m_sendInterval = std::chrono::seconds(1);
	m_timeOutInterval = std::chrono::seconds(2);

	// compute and set identifier
	identifier = 0xA8A8;
	requestBody.resize(s_dataBufferSize, 0xFF);

	bufsz = 66000;
	recvbuff.resize(bufsz, 0);
}

network::Pinger::~Pinger() {}

void network::Pinger::AddHost(std::string &host,  unsigned key) {

	icmp::endpoint dest =  *host_resolver.resolve(icmp::v4(), host, "").begin();
	boost::asio::steady_timer *timer = new boost::asio::steady_timer(m_ioc);
	HostInfo hi(timer, dest, host);

	m_pMutex->lock();
	m_pHosts->insert_or_assign(key, hi);
	m_pMutex->unlock();
	std::cout << "adding host: " << hi.GetDestination()
		  << " with key " << key << std::endl;
}

void network::Pinger::StartPing() {
	std::cout << "network::Pinger::StartPing()" << std::endl;
	for (auto it = m_pHosts->begin(); it != m_pHosts->end(); it++) {
		startSend(it->first);
	}
	startReceive();
}

void network::Pinger::StartHostPing(unsigned key, bool receive = false) {
	startSend(key);
	if (receive) { startReceive(); }
}

void network::Pinger::fillDataBuffer(uint32_t id, uint32_t seq, uint32_t nHost) {
	if (requestBody.size() < 3 * sizeof(uint32_t)) return;
	uint32_t *array = reinterpret_cast<uint32_t *> (requestBody.data());
	array[0] = id;
	array[1] = seq;
	array[2] = nHost;
}

void network::Pinger::parseDataBuffer(uint32_t &id, uint32_t &seq, uint32_t &nHost) {
	if (requestBody.size() < 3 * sizeof(uint32_t)) return;
	uint32_t *array = reinterpret_cast<uint32_t *> (requestBody.data());
	id = array[0];
	seq = array[1];
	nHost = array[2];
}

void network::Pinger::startSend(unsigned key) {
	m_pMutex->lock();
	if (!m_pHosts->contains(key)) { return; }
	HostInfo &h = m_pHosts->at(key);

	ICMP4Proto protocol;
	requestBody.resize(s_dataBufferSize, 0xFF);
	fillDataBuffer(identifier, h.sequence, key);

	std::vector<uint8_t> packet = protocol.CreateEchoPacket(requestBody,
								identifier,
								h.sequence);

	std::cerr << ">>>> sending " << packet.size() << " bytes, Sequence "
		  << h.sequence << ", to: "
		  << h.GetDestination()
		  << ", state " << key << std::endl;

	sock.send_to(boost::asio::buffer(packet), h.GetDestination());
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	h.TimeSent(now); // sets reply_received=false

	h.m_stimer->expires_after(m_timeOutInterval);
	h.m_stimer->async_wait( [&, key] (const boost::system::error_code& error)
	{
		if (error) {
			// print error only when timer isn't cancelled
			// std::cerr << "timed out timer error "
			// 	  << error.message() << std::endl;
		}
		this->timeOut(key);
	});

	m_pMutex->unlock();
}

//
// Rename all these handlers/funcitons etc
//
// 

void network::Pinger::timeOut(unsigned key) {
	m_pMutex->lock();
	if (!m_pHosts->contains(key)) { return; }
	HostInfo &h = m_pHosts->at(key);

	HostInfo::ping_reply pr;
	// if haven't received, push reply structure set to timed out

	if (!h.reply_received) {
		pr.status = HostInfo::Timeout;
		pr.sequence = h.sequence;
		h.PushReply(pr);
	}

	h.m_stimer->expires_after(m_sendInterval);
	h.m_stimer->async_wait( [&, key] (const boost::system::error_code& error)
	{
		if (error) {
			// std::cerr << "send timer error "
			// 	  << error.message() << std::endl;
			return;
		}
		this->startSend(key);
	});

	m_pMutex->unlock();
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
	ipv4_header ip_header;
	ICMP4Proto protocol;
	unsigned data_offset;
	HostInfo::ping_reply pr;

	std::cerr << "<<<< Received " << size << " bytes" << std::endl;

	if (size == 0) {
		startReceive();
		return;
	}

	// time for latency calculation
	std::chrono::steady_clock::time_point time_received
		= std::chrono::steady_clock::now();

	// parse IP header
	int ip_size = ip_header.ParsePacket(recvbuff);

	// parse ICMP header
	std::vector<uint8_t> icmp_content(recvbuff.begin() + ip_size, recvbuff.end());
	ICMP4Proto::Type icmp_type = protocol.ParseReply(icmp_content, data_offset);

	requestBody.assign(icmp_content.begin() + data_offset, icmp_content.end());

	// reverse dns resolve remote host
	icmp::endpoint remote_ep;
	remote_ep.address(ip_header.source_address());
	std::string remote_hostname = host_resolver.resolve(remote_ep).begin()->host_name();

	if (icmp_type != ICMP4Proto::EchoReply) {
		// not an echo reply packet,
		// the ping_reply struct will be filled out appropriately
		// in the timeout?
		std::cerr << "Received not echo: " << icmp_type << std::endl;
	} else {
		// this is an echo reply
		//
		// check the packet checksum?? (at least icmp)
		uint32_t host_key, data_id, data_seq;
		uint16_t packet_id = protocol.GetIdentifier();
		parseDataBuffer(data_id, data_seq, host_key);

		m_pMutex->lock();
		if (packet_id == identifier
		    && m_pHosts->contains(host_key) ) {
		
			HostInfo &h = m_pHosts->at(host_key);

			pr.latency = time_received - h.time_last_sent;
			pr.status = HostInfo::Reply;
			pr.time_to_live = ip_header.time_to_live();
			pr.sequence = protocol.GetSequenceNumber();
			pr.remote_ip = ip_header.source_address();
			pr.remote_hostname = remote_hostname;

			// testing
			std::cout << "\t Sequence " << pr.sequence
				  << ", Latency "
				  << pr.latency.count() 
				  << " milliseconds, ttl " << pr.time_to_live
				  << " from " << pr.remote_hostname
				  << " (" << pr.remote_ip.to_string() << ')'
				  << " key: " << host_key << std::endl;
			// % loss

			h.PushReply(pr);
			h.m_stimer->cancel();
		}
		m_pMutex->unlock();
	}

	startReceive();
}
