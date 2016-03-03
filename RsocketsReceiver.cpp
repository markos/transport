#include <cassert>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <system_error>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <rdma/rdma_cma.h>
#include <rdma/rsocket.h>

#include <capnp/serialize.h>

#include "common.h"
#include "utils.h"
#include "ReceiverStats.h"
#include "RsocketsReceiver.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

RsocketsReceiver::RsocketsReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool)
: Receiver(i, address, port, pool) {
	cout << "Starting RDMA Receiver on " << serverIP << ", port " << serverPort << endl;
	
	int rc;
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	servAddr.sin_port = htons(port +i);
		
	cout << "Thread " << index << ": Binding to " << endPoint.str() << "..." << endl;
	sock = rsocket(AF_INET, SOCK_STREAM, 0);
	assert(sock);

	rc = rbind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
	assert(rc == 0);

	rc = rlisten(sock,1);
	assert(rc == 0);
}

void
RsocketsReceiver::operator()() {
	parentPool.register_stats(&stats);
	try {
		int received;
		int new_sock;
		struct sockaddr_in client_addr;
		unsigned int client_addr_len;
		
		new_sock = accept(sock, (struct sockaddr *) &client_addr, &client_addr_len);
		assert(new_sock);

		// receive the message
		cout << "Thread " << index << ": Receiving messages..." << endl;

		struct pollfd fds;
		fds.events = POLLIN;
		fds.fd = new_sock;

		while(true) {
			int rc = rpoll(&fds, 1, -1);
			assert(rc != 1);

			received = rrecv(fds.fd, buffer, BUFFER_SIZE, flags);
			if (received) {
				cout << "received " << received << " bytes" << endl;
				hexdump(buffer, received);
			}
		}
		cout << "Finished." << endl;
	} catch (const std::system_error &e) {
		std::cerr << e.what() << std::endl;
		return;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

}