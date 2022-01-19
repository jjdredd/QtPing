#include <icmp4.hpp>
#include <algorithm>

network::ICMP4Proto::ICMP4TCCHeader::ParseTCC(uint8_t *buf) {
	type = buf[0];
	code = buf[1];
	checksum = *(static_cast<uint16_t *>(&buf[2]));
}

network::ICMP4Proto::ICMP4Proto() {}
network::ICMP4Proto::~ICMP4Proto() {}

bool network::ICMP4Proto::ParseForEchoReply(std::vector<uint8_t> &packet, unsigned &Ndata) {

}

network::ICMP4Proto::Type network::ICMP4Proto::ParseReply(std::vector<uint8_t> &packet,
							  unsigned &Ndata) {

	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);
	switch(tcc.type) {

	case EchoReply:
		Ndata = parseEchoReply();
		break;
		
	case DestinationUnreachable:

		break;

	case SourceQuench:

		break;
	case Redirect:

		break;
	case Echo:

		break;

	case TimeExceeded:

		break;

	case ParameterProblem:

		break;
	case Timestamp:

		break;
	case TimestampReply:

		break;
	case InfoRequest:

		break;
	case InfoReply:

		break;
	}

	return tcc.type;
}

std::vector<uint8_t> network::ICMP4Proto::CreateEchoPacket(std::vector<uint8_t> &data,
							   uint8_t id, uint8_t seqn) {

	unsigned header_size = tcc::size + 2*sizeof(uint16_t);
	std::vector<uint8_t> packet(header_size + data.size());
	packet[0] = Echo;	// type
	packet[1] = 0;		// code
	*(static_cast<uint16_t *>(&packet[2])) = id;
	*(static_cast<uint16_t *>(&packet[2 + sizeof(uint16_t)])) = seqn;
	std::copy(data.begin(), data.end(), packet.begin() + header_size);
	return packet;
}


std::vector<uint8_t> network::ICMP4Proto::CreateInfoRequestPacket() {
	return std::vector<uint8_t>();
}


std::vector<uint8_t> network::ICMP4Proto::CreateTimestampPacket() {
	return std::vector<uint8_t>();
}


// private functions
uint8_t network::ICMP4Proto::parseEchoReply(std::vector<uint8_t> &packet,
					    uint16_t &id, uint16_t &seqn) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);
	if (tcc.code != 0) { return -1 };

	id = *(static_cast<uint16_t *>(&packet[tcc.size]));
	seqn = *(static_cast<uint16_t *>(&packet[tcc.size + sizeof(uint16_t)]));
	return tcc.size + 2*sizeof(uint16_t);
}


uint8_t network::ICMP4Proto::parseDestinationUnreachable(std::vector<uint8_t> &packet,
							 std::string &reason) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);
	
	switch (tcc.code) {

	case 0:
		reason = "net unreachable";
		break;
	case 1:
		reason = "host unreachable";
		break;
	case 2:
		reason = "protocol unreachable";
		break;
	case 3:
		reason = "port unreachable";
		break;
	case 4:
		reason = "fragmentation needed and DF set";
		break;
	case 5:
		reason = "source route failed";
		break;
	}
}


uint8_t network::ICMP4Proto::parseSourceQuench(std::vector<uint8_t> &packet) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);

	return tcc::size + 4;
}


uint8_t network::ICMP4Proto::parseRedirect(std::vector<uint8_t> &packet) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);

	return tcc::size + 4;
}


uint8_t network::ICMP4Proto::parseTimeExceeded(std::vector<uint8_t> &packet) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);

	return tcc::size + 4;
}


uint8_t network::ICMP4Proto::parseParameterProblem(std::vector<uint8_t> &packet) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);

	return tcc::size + 4;
}


void network::ICMP4Proto::parseTimestampReply(std::vector<uint8_t> &packet
					      /*, timestamsp, id, seqn */) {
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);

	if (tcc.code != 14) { return -1; }

	return tcc::size + 4;
}


void network::ICMP4Proto::parseInfoReply(std::vector<uint8_t> &packet) {
	// id and seqn in arguments
	ICMP4TCCHeader tcc;
	tcc.ParseTCC(packet);

	if (tcc.code != 16) { return -1; }

	return tcc::size + 4;
}

