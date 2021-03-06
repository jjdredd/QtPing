#include <icmp4.hpp>
#include <algorithm>
#include <iostream>


static uint16_t word_endian(uint16_t a) {
	uint16_t upper = a & 0xFF00;
	uint16_t lower = a & 0xFF;
	return (lower << 8) | (upper >> 8);
}


network::ICMP4Proto::ICMP4Proto()
	: state(false), type(0), code(0), checksum(0), n_data(0)
	, id(0), seqn(0) {}


network::ICMP4Proto::~ICMP4Proto() {}


void network::ICMP4Proto::parseTCC(std::vector<uint8_t> &buf) {
	type = buf[0];
	code = buf[1];
	checksum = word_endian(*(reinterpret_cast<uint16_t *>(&buf[2])));
}


network::ICMP4Proto::Type network::ICMP4Proto::ParseReply(std::vector<uint8_t> &packet,
							  unsigned & data_offset) {

	ResetState();
	parseTCC(packet);

	switch(type) {

	case EchoReply:
		data_offset = parseEchoReply(packet);
		break;
		
	case DestinationUnreachable:
		data_offset = parseDestinationUnreachable(packet);
		break;

	case SourceQuench:
		data_offset = parseSourceQuench(packet);
		break;

	case Redirect:
		data_offset = parseRedirect(packet);
		break;

	case Echo:
		// we parse only reply for this application
		break;

	case TimeExceeded:
		data_offset = parseTimeExceeded(packet);
		break;

	case ParameterProblem:
		data_offset = parseParameterProblem(packet);
		break;

	case Timestamp:
		// we parse only reply for this application
		break;

	case TimestampReply:
		data_offset = parseTimestampReply(packet);
		break;

	case InfoRequest:
		// we parse only reply for this application
		break;

	case InfoReply:
		data_offset = parseInfoReply(packet);
		break;
	}

	return static_cast<network::ICMP4Proto::Type>(type);
}

std::vector<uint8_t> network::ICMP4Proto::CreateEchoPacket(std::vector<uint8_t> &data,
							   uint16_t id, uint16_t seqn) {

	unsigned header_size = tcc_size + 2*sizeof(uint16_t);
	std::vector<uint8_t> packet(header_size + data.size());
	packet[0] = Echo;	// type
	packet[1] = 0;		// code
	*(reinterpret_cast<uint16_t *>(&packet[2])) = 0; // checksum
	*(reinterpret_cast<uint16_t *>(&packet[tcc_size])) = word_endian(id);
	*(reinterpret_cast<uint16_t *>(&packet[tcc_size + sizeof(uint16_t)]))
		= word_endian(seqn);

	if (data.size() > 0) {
		std::copy(data.begin(), data.end(), packet.begin() + header_size);
	}

	// compute checksum and fill it in
	*(reinterpret_cast<uint16_t *>(&packet[2])) = word_endian(computeChecksum(packet));

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

bool network::ICMP4Proto::GetState() const { return state; }

void network::ICMP4Proto::ResetState() {
	state = false;
	type = code = checksum = 0;
	n_data = gateway_address = 0;
	id = seqn = 0;
	originate_ts = receive_ts = transmit_ts = 0;
	reason.clear();
}

network::ICMP4Proto::Type network::ICMP4Proto::GetType() const {
	return static_cast<network::ICMP4Proto::Type>(type);
}

uint8_t network::ICMP4Proto::GetCode() const { return code; }
uint16_t network::ICMP4Proto::GetChecksum() const { return checksum; }
unsigned network::ICMP4Proto::GetDataOffset() const { return n_data; }
uint16_t network::ICMP4Proto::GetIdentifier() const { return id; }
uint16_t network::ICMP4Proto::GetSequenceNumber() const { return seqn; }


// 
// private functions
// 
uint8_t network::ICMP4Proto::parseEchoReply(std::vector<uint8_t> &packet) {
	if (code != 0) { return -1; };

	id = word_endian(*(reinterpret_cast<uint16_t *>(&packet[tcc_size])));
	seqn = word_endian(*(reinterpret_cast<uint16_t *>
			     ( &packet[tcc_size + sizeof(uint16_t)] )));
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


uint8_t network::ICMP4Proto::parseTimestampReply(std::vector<uint8_t> &packet
					      /*, timestamsp, id, seqn */) {

	if (code != 14) { return 0; }

	return n_data = tcc_size + 4;
}


uint8_t network::ICMP4Proto::parseInfoReply(std::vector<uint8_t> &packet) {
	// id and seqn in arguments

	if (code != 16) { return 0; }

	return n_data = tcc_size + 4;
}

uint16_t network::ICMP4Proto::computeChecksum(std::vector<uint8_t> &packet) {
	// compute checksum by copying the packet data and setting
	// the checksum field to zero
	
	std::vector<uint8_t> buf(packet);
	uint16_t *array = reinterpret_cast<uint16_t *>(buf.data());
	uint16_t res;
	int size = buf.size() / 2;
	int left = buf.size() % 2;

	unsigned int sum = 0;
	unsigned n = 0;

	array[1] = 0;		// set checksum field (specification)
	for (n = 0; n < size; n++) {
		sum += word_endian(array[n]);
	}

	if (left != 0) {
		std::cerr << "odd size in checksum" << std::endl;
		uint16_t last_byte = *(reinterpret_cast<uint8_t *>(&array[n]));
		sum += (last_byte << 8);
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	res = ~sum;
	return res;
}
