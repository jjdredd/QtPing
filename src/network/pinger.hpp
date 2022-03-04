// struct to hold all the reply status / etc (?)
// class to hold all the host info with replies(?) (not but almost ring buffer)
// pinger class to ping all the hosts

#include <vector>
#include <chrono>
#include <string>

#include <boost/system/error_code.hpp>

#include "ipv4.hpp"


using boost::asio::ip::icmp;
using boost::asio::steady_timer;

class Pinger;

namespace network {

	// 
	// class to hold all the host info regarding connectivity
	// 
	class HostInfo {

		friend class Pinger;

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

		std::vector<ping_reply> replies;
		// num replies (?)
	};


	//
	// Pinger class manages a vector hostinfo and sends and receives icmp probes
	// 

	class Pinger {

	public:
		Pinger(boost::asio::io_context &);
		virtual ~Pinger();

		void AddHost(std::string &);
		bool DeleteHost(unsigned);
		// double GetHostLatency();
		// double GetHostMinLatency();
		// double GetHostMaxLatency();
		// double GetHostStdDev();
		// std::string GetHostName();
		// std::string GetHostAddressString();
		std::vector<HostInfo::ping_reply> GetHostReplies(unsigned);

	private:
		icmp::endpoint resolveHostOrIP(std::string &);

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

		std::vector<HostInfo> remote_hosts;
		std::vector<uint8_t> recvbuff, requestBody;
		unsigned bufsz;

		static const unsigned s_dataBufferSize = 56;
	};

}
