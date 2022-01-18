// the icmp4 packet acording to rfc 792
// https://datatracker.ietf.org/doc/html/rfc792


 /////////////////////////////////////////////////////////////////////////////
 // the values of the internet header					    //
 //   fields are as follows:						    //
 // 									    //
 //   Version								    //
 // 									    //
 //      4								    //
 // 									    //
 //   IHL								    //
 // 									    //
 //      Internet header length in 32-bit words.			    //
 // 									    //
 //   Type of Service							    //
 // 									    //
 //      0								    //
 // 									    //
 //   Total Length							    //
 // 									    //
 //      Length of internet header and data in octets.			    //
 // 									    //
 //   Identification, Flags, Fragment Offset				    //
 // 									    //
 //      Used in fragmentation, see [1].				    //
 // 									    //
 //   Time to Live							    //
 // 									    //
 //      Time to live in seconds; as this field is decremented at each	    //
 //      machine in which the datagram is processed, the value in this	    //
 //      field should be at least as great as the number of gateways which  //
 //      this datagram will traverse.					    //
 // 									    //
 //   Protocol								    //
 // 									    //
 //      ICMP = 1							    //
 // 									    //
 //   Header Checksum							    //
 // 									    //
 //      The 16 bit one's complement of the one's complement sum of all 16  //
 //      bit words in the header.  For computing the checksum, the checksum //
 //      field should be zero.  This checksum may be replaced in the	    //
 //      future.							    //
 /////////////////////////////////////////////////////////////////////////////


#pragma once

#include <cstdint>
#include <vector>


namespace network {

	class ICMP4Proto {

		enum Type : uint8_t {
			EchoReply = 0,
			DestinationUnreachable = 3,
			SourceQuench = 4,
			Redirect = 5,
			Echo = 8,
			TimeExceeded = 11,
			ParameterProblem = 12,
			Timestamp = 13,
			TimestampReply = 14,
			InfoRequest = 15,
			InfoReply = 16,
		};

	public:
		ICMP4Packet();
		virtual ~ICMP4Packet();

		Type ParseReply(std::vector<uint8_t> &);

		std::vector<uint8_t> CreateEchoPacket();
		std::vector<uint8_t> CreateInfoRequestPacket();
		std::vector<uint8_t> CreateTimestampPacket();

	private:
		std::vector<uint8_t> parseEchoReply();
		std::vector<uint8_t> parseDestinationUnreachable();
		void parseSourceQuench();
		std::vector<uint8_t> parseRedirect(); // use std string?
		std::vector<uint8_t> parseTimeExceeded(); // use std string?
		void parseParameterProblem();
		void parseTimestampReply(/* timestamsp */);
		void parseInfoReply();
	};

}
