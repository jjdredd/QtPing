#include <cstdint>
#include <sys/socket.h>
#include <sys/uio.h>
#include <vector>

namespace network {

	class Socket {

	public:
		Socket();	// destination address, etc
		~Socket();

		void ReceivePacket(); // get all the useful packet info here

	private:
		struct msghdr msg;
		struct sockaddr src_addr, host_addr;
		char address_string[128];
		uint8_t ansillary[4096];
		struct iovec iov;
	};

}
