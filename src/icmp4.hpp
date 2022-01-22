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
#include <string>


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
		ICMP4Proto();
		virtual ~ICMP4Proto();

		Type ParseReply(std::vector<uint8_t> &, unsigned &);

		std::vector<uint8_t> CreateEchoPacket(std::vector<uint8_t> &,
						      uint16_t, uint16_t);
		std::vector<uint8_t> CreateInfoRequestPacket();
		std::vector<uint8_t> CreateTimestampPacket();

		// internal state
		bool GetState() const;
		void ResetState();

		Type GetType() const;
		uint8_t GetCode() const;
		uint16_t GetChecksum() const;
		unsigned GetDataOffset() const;

		uint16_t GetIdentifier() const;
		uint16_t GetSequenceNumber() const;

	private:
		void parseTCC(std::vector<uint8_t> &);
		
		uint8_t parseEchoReply(std::vector<uint8_t> &);
		uint8_t parseDestinationUnreachable(std::vector<uint8_t> &);
		uint8_t parseSourceQuench(std::vector<uint8_t> &);
		uint8_t parseRedirect(std::vector<uint8_t> &);
		uint8_t parseTimeExceeded(std::vector<uint8_t> &); // use std string?
		uint8_t parseParameterProblem(std::vector<uint8_t> &);
		void parseTimestampReply(std::vector<uint8_t> & /*, timestamsp */);
		void parseInfoReply(std::vector<uint8_t> &);

		uint16_t computeChecksum(std::vector<uint8_t> &);

		// internal state variables
		bool state; // true if internal vars are filled in

		// TCC - type, code, checksum (present in every ICMP packet
		uint8_t type, code;
		uint16_t checksum;
		static const unsigned tcc_size = 4; // four bytes

		uint16_t n_data; //   data offset
		uint32_t gateway_address; // for RedirectMessage
		uint16_t id, seqn;	  // for Echo or EchoReply, InfoReq, InfoRep
		uint32_t originate_ts, receive_ts, transmit_ts; // timestamps

		std::string reason // for DestinationUnreachable
	};

}
