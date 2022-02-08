#include <icmp4.hpp>
#include <algorithm>


network::ICMP4Proto::ICMP4Proto()
	: i_var_state(false), type(0), code(0), checksum(0), n_data(0)
	, id(0), seqn(0) {}


network::ICMP4Proto::~ICMP4Proto() {}


network::ICMP4Proto::parseTCC(std::vector<uint8_t> &buf)) {
	type = buf[0];
	code = buf[1];
	checksum = *(static_cast<uint16_t *>(&buf[2]));
}


network::ICMP4Proto::Type network::ICMP4Proto::ParseReply(std::vector<uint8_t> &packet) {

	ResetState();
	parseTCC(packet);

	switch(type) {

	case EchoReply:
		parseEchoReply(packet);
		break;
		
	case DestinationUnreachable:
		parseDestinationUnreachable(packet);
		break;

	case SourceQuench:
		parseSourceQuench(packet);
		break;

	case Redirect:
		parseRedirect(packet);
		break;

	case Echo:
		// we parse only reply for this application
		break;

	case TimeExceeded:
		parseTimeExceeded(packet);
		break;

	case ParameterProblem:
		parseParameterProblem(packet);
		break;

	case Timestamp:
		// we parse only reply for this application
		break;

	case TimestampReply:
		parseTimestampReply(packet);
		break;

	case InfoRequest:
		// we parse only reply for this application
		break;

	case InfoReply:
		parseInfoReply(packet);
		break;
	}

	return type;		// change (?), we still need type (?)
}

std::vector<uint8_t> network::ICMP4Proto::CreateEchoPacket(std::vector<uint8_t> &data,
							   id, seqn) {

	unsigned header_size = tcc_size + 2*sizeof(uint16_t);
	std::vector<uint8_t> packet(header_size + data.size());
	packet[0] = Echo;	// type
	packet[1] = 0;		// code
	*(static_cast<uint16_t *>(&packet[2])) = 0; // checksum
	*(static_cast<uint16_t *>(&packet[tcc_size])) = id;
	*(static_cast<uint16_t *>(&packet[tcc_size + sizeof(uint16_t)])) = seqn;

	if (data.size() > 0) {
		std::copy(data.begin(), data.end(), packet.begin() + header_size);
	}

	// compute checksum and fill it in
	*(static_cast<uint16_t *>(&packet[2])) = computeChecksum(packet); // checksum

	return packet;
}


std::vector<uint8_t> network::ICMP4Proto::CreateInfoRequestPacket() {
	return std::vector<uint8_t>();
}


std::vector<uint8_t> network::ICMP4Proto::CreateTimestampPacket() {
	return std::vector<uint8_t>();
}


// 
// Get internal state
// 

bool network::ICMP4Proto::GetState() { return state; }

void network::ICMP4Proto::ResetState() {
	state = false;
	type = code = checksum = 0;
	n_data = gateway_address = 0;
	id = seqn = 0;
	originate_ts = receive_ts = transmit_ts = 0;
	reason.clear();
}

Type network::ICMP4Proto::GetType() const { return type; }
uint8_t network::ICMP4Proto::GetCode() const { return code; }
uint16_t network::ICMP4Proto::GetChecksum() const { return checksum; }
unsigned network::ICMP4Proto::GetDataOffset() const { return n_data; }
uint16_t network::ICMP4Proto::GetIdentifier() const { return id; }
uint16_t network::ICMP4Proto::GetSequenceNumber() const { return seqn; }


// 
// private functions
// 
uint8_t network::ICMP4Proto::parseEchoReply(std::vector<uint8_t> &packet) {
	if (code != 0) { return -1 };

	id = *(static_cast<uint16_t *>(&packet[tcc.size]));
	seqn = *(static_cast<uint16_t *>(&packet[tcc.size + sizeof(uint16_t)]));
	return n_data = tcc_size + 2*sizeof(uint16_t);
}


uint8_t network::ICMP4Proto::parseDestinationUnreachable(std::vector<uint8_t> &packet) {
	switch (code) {

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
	return n_data = 4* sizeof(uint8_t) + tcc_size;
}


uint8_t network::ICMP4Proto::parseSourceQuench(std::vector<uint8_t> &packet) {

	return n_data = tcc_size + 4;
}


uint8_t network::ICMP4Proto::parseRedirect(std::vector<uint8_t> &packet) {

	return n_data = tcc_size + 4;
}


uint8_t network::ICMP4Proto::parseTimeExceeded(std::vector<uint8_t> &packet) {

	return n_data = tcc_size + 4;
}


uint8_t network::ICMP4Proto::parseParameterProblem(std::vector<uint8_t> &packet) {

	return n_data = tcc_size + 4;
}


void network::ICMP4Proto::parseTimestampReply(std::vector<uint8_t> &packet
					      /*, timestamsp, id, seqn */) {

	if (code != 14) { return -1; }

	return n_data = tcc_size + 4;
}


void network::ICMP4Proto::parseInfoReply(std::vector<uint8_t> &packet) {
	// id and seqn in arguments

	if (code != 16) { return -1; }

	return n_data = tcc_size + 4;
}

uint16_t network::ICMP4Proto::computeChecksum(std::vector<uint8_t> &packet) {
	// compute checksum by copying the packet data and setting
	// the checksum field to zero
	
	std::vector<uint8_t> buf(packet);
	uint16_t *array = buf.data();
	uint16_t res;
	int size = buf.size() / 2;
	int left = buf.size() % 2;

	int sum = 0;

	array[1] = 0;		// set checksum field (specification)
	for (unsigned n = 0; n < size; n++) {
		sum += array[n];
	}

	if (left != 0) {
		sum += *(static_cast<uint8_t *>(&array[n]));
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	res = ~sum;
	return res;
}
