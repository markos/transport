#include <string>
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include <system_error>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <rdma/rdma_cma.h>
#include <rdma/rsocket.h>

#include <capnp/serialize.h>

#include "utils.h"
#include "RsocketsTransmitter.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

RsocketsTransmitter::RsocketsTransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool)
: Transmitter(i, address, port, pool) {
	cout << "Connecting Rsockets Transmitter to " << serverIP << ", port " << serverPort << endl;

	int rc;

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	servAddr.sin_port = htons(serverPort);
	
	cout << "Thread " << index << ": Connecting to " << endPoint.str() << "..." << endl;
	sock = rsocket(AF_INET, SOCK_STREAM, 0);
	assert(sock);

	rc = rconnect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
	assert(rc == 0);
}

void
RsocketsTransmitter::operator()() {
	try {
		// send the messages
		cout << "Thread " << index << ": Sending messages..." << endl;
		int flags{0};
		int sent;

		struct pollfd fds;
		fds.events = POLLOUT;
		fds.fd = sock;

		while(true) {
			int rc = rpoll(&fds, 1, -1);
			assert(rc != 1);

			MessageItem rawMsg;
			parentPool.msgQueue.wait_dequeue(rawMsg);
			if (rawMsg.first != nullptr && rawMsg.second != 0) {

				sent = rsend(sock, rawMsg.first, rawMsg.second, flags);
				assert(sent != -1);
			} else {
				break;
			}
		}
		cout << "Finished." << endl;
	} catch (const std::system_error &e) {
		std::cerr << e.what() << std::endl;
		return;
	}
}

}