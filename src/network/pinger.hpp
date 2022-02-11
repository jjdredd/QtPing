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
namespace chrono = boost::asio::chrono;

namespace network {

	// 
	// class to hold all the host info regarding connectivity
	// 
	class HostInfo {

		enum reply_status {
			Reply,
			Unreachable,
			Timeout,
			// add more later
		};

		struct ping_reply {
			reply_status status;
			std::chrono::duration latency;
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
		void TimeSent(chrono::steady_clock::time_point &);
		icmp::endpoint GetDestination() const ;
		bool ReplyReceived() const;

	private:

		void computeStats(ping_reply &);

		double mean_latency, stdev_latency; // use stats class
		icmp::endpoint destination;
		unsigned sequence;
		chrono::steady_clock::time_point time_last_sent;
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

	private:
		icmp::endpoint resolveHostOrIP(std::string &);
		void startSend();
		void startReceive();
		void timeOut();
		void receive();

		icmp::resolver host_resolver;
		icmp::socket sock;
		uint8_t identifier; // uint8_t??
		steady_timer stimer;
		// reply buffer?

		std::vector<HostInfo> remote_hosts;
	};

}
