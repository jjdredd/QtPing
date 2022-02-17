#include <iostream>

#include "pinger.hpp"

int main(int argc, char *argv[]) {
	std::string remote_host("8.8.8.8");
	boost::asio::io_context ioc;
	Pinger p(ioc);

	p.AddHost(remote_host);

	ioc.run();

	return 0;
}
