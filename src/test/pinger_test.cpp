#include <iostream>

#include "pinger.hpp"

int main(int argc, char *argv[]) {
	std::string remote_host_1("66.163.70.130"), remote_host_2("gw-cloudflare.yycix.ca");
	boost::asio::io_context ioc;
	network::Pinger p(ioc);

	p.AddHost(remote_host_1);
	p.AddHost(remote_host_2);

	ioc.run();

	return 0;
}
