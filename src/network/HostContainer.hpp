// A container aggregating HostInfo
// classes with additional service functions
// like add and delete, id management
//
// This container must be thread-safe

#pragma once

#include <vector>
#include <chrono>
#include <string>
#include <mutex>

#include <boost/system/error_code.hpp>

using boost::asio::ip::icmp;
using boost::asio::steady_timer;

class HostContainer;

namespace network {

	// 
	// class to hold all the host info regarding connectivity
	// 
	class HostInfo {

		friend class HostContainer;

		enum reply_status {
			Reply,
			Unreachable,
			Timeout,
			// add more later
		};

		struct ping_reply {
			reply_status status;
			std::chrono::duration<double, std::milli> latency;
			unsigned int time_to_live;
			unsigned int sequence;
			boost::asio::ip::address_v4 remote_ip;
			std::string remote_hostname;
		};

	public:
		HostInfo(icmp::endpoint &, std::string &);
		virtual ~HostInfo();

		std::vector<ping_reply> GetReplies(); // std::swap or std::move vectors
		void PushReply(ping_reply &);
		void TimeSent(std::chrono::steady_clock::time_point &);
		icmp::endpoint GetDestination() const ;

	private:

		void computeStats(ping_reply &);

		double mean_latency, stdev_latency; // use stats class
		icmp::endpoint destination;
		unsigned sequence;
		std::chrono::steady_clock::time_point time_last_sent;
		bool reply_received; // sent and received (false if not sent) or timeout
		std::string host_string;

		// identifier will be the index in the hostinfo array in Pinger class

		// better have a list?
		std::vector<ping_reply> replies;
		// num replies (?)
	};

	//
	// HostContainer class
	// 

	class HostContainer {

	public:
		HostContainer();
		virtual ~HostContainer();

	private:
		std::mutex m_mutex;
		std::unordered_map<unsigned, HostInfo> m_hosts;
		unsigned keys;

	};

}
