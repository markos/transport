#include <string>
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include <system_error>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <mellanox/vma_extra.h>

#include <capnp/serialize.h>

#include "utils.h"
#include "VMATransmitter.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

VMATransmitter::VMATransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool)
: Transmitter(i, address, port, pool) {
	cout << "Connecting VMA Transmitter to " << serverIP << ", port " << serverPort << endl;

	int rc;

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	servAddr.sin_port = htons(serverPort);
		
	cout << "Thread " << index << ": Connecting to " << endPoint.str() << "..." << endl;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock);

	rc = connect(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
	assert(rc == 0);

	//vma_api = vma_get_api();
	//assert(vma_api == nullptr);

	cout << "VMA API available" << endl;
}

void
VMATransmitter::operator()() {
	try {
		// send the messages
		cout << "Thread " << index << ": Sending messages..." << endl;
		int flags = MSG_VMA_ZCOPY_FORCE;
		int rc;

		while(true) {
			MessageItem rawMsg;
			parentPool.msgQueue.wait_dequeue(rawMsg);
			if (rawMsg.first != nullptr && rawMsg.second != 0) {
				rc = send(sock, rawMsg.first, rawMsg.second, flags);
				assert(rc != -1);
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