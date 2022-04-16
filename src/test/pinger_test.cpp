#include <iostream>
#include <thread>
#include <chrono>

#include "pinger.hpp"

int main(int argc, char *argv[]) {
	std::string remote_host_1("66.163.70.130"), remote_host_2("gw-cloudflare.yycix.ca");

	boost::asio::io_context ioc;
	std::mutex mtx;
	std::unordered_map<unsigned, network::HostInfo> him;

	network::Pinger p(&him, &mtx, ioc);

	std::thread th1( [&] () {ioc.run();} );

	// add host 1
	{
		std::cout << std::endl
			  << "Adding host (" << remote_host_1 << ")"
			  << std::endl;

		p.AddHost(remote_host_1, 1);
	}

	std::this_thread::sleep_for(std::chrono::seconds(17));
	// add host 2 after a while
	{
		std::cout << std::endl
			  << "Adding host (" << remote_host_2 << ")"
			  << std::endl;

		p.AddHost(remote_host_2, 2);
	}

	std::this_thread::sleep_for(std::chrono::seconds(29));
	// delete host 2 after a while
	{
		std::cout << std::endl
			  << "deleting host (2)"
			  << std::endl;

		const std::lock_guard<std::mutex> lock(mtx);
		him.erase(2);
	}

	std::this_thread::sleep_for(std::chrono::seconds(11));
	ioc.stop();
	th1.join();
	return 0;
}
