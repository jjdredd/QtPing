#include <iostream>

#include "pinger.hpp"

int main(int argc, char *argv[]) {
	std::string remote_host("64.59.184.253");
	boost::asio::io_context ioc;
	network::Pinger p(ioc);

	p.AddHost(remote_host);

	ioc.run();

	return 0;
}
