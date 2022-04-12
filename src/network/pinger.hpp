// struct to hold all the reply status / etc (?)
// class to hold all the host info with replies(?) (not but almost ring buffer)
// pinger class to ping all the hosts

#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <memory>

#include <boost/system/error_code.hpp>

#include "ipv4.hpp"
#include "HostContainer.hpp"

using boost::asio::ip::icmp;
using boost::asio::steady_timer;

class Pinger;

namespace network {

	//
	// Pinger class manages a vector hostinfo and sends and receives icmp probes
	// 

	class Pinger {

	public:
		Pinger(std::shared_ptr<HostContainer> &, boost::asio::io_context &);
		virtual ~Pinger();

	private:

		// packet additional data
		void fillDataBuffer(uint32_t, uint32_t, uint32_t);
		// we can add new id, sequence, number of host and timestamp
		// to the data buffer to send with icmp echo
		// (maybe withoout timestamp at first)
		void parseDataBuffer(uint32_t &, uint32_t &, uint32_t &);

		void startSend();
		void startReceive();
		void timeOut();
		void receive(std::size_t);

		icmp::resolver host_resolver;
		icmp::socket sock;
		uint16_t identifier;
		boost::asio::steady_timer stimer;

		std::shared_ptr<HostContainer> m_hostContainer;
		std::vector<uint8_t> recvbuff, requestBody;
		unsigned bufsz;

		static const unsigned s_dataBufferSize = 56;
	};

}
