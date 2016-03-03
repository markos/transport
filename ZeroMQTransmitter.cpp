#include <string>
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include <system_error>

#include <zmqpp/zmqpp.hpp>

#include <capnp/serialize.h>

#include "utils.h"
#include "ZeroMQTransmitter.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

ZeroMQTransmitter::ZeroMQTransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool)
: Transmitter(i, address, port, pool) {
	cout << "Connecting ZeroMQ Transmitter to " << serverIP << ", port " << serverPort << endl;
}

void
ZeroMQTransmitter::operator()() {
	try {
		// initialize the 0MQ context
		zmqpp::context context;

		// generate a push socket
		zmqpp::socket_type type = zmqpp::socket_type::push;
		zmqpp::socket sock (context, type);
		
		cout << "Thread " << index << ": Connecting to " << endPoint.str() << "..." << endl;
		sock.connect(endPoint.str());
		
		// send the messages
		cout << "Thread " << index << ": Sending messages..." << endl;
		zmqpp::message* msg;
		while(true) {
			MessageItem rawMsg;
			parentPool.msgQueue.wait_dequeue(rawMsg);
			if (rawMsg.first != nullptr && rawMsg.second != 0) {
				msg = new zmqpp::message();
				msg->add_raw(reinterpret_cast<void*>(rawMsg.first), rawMsg.second);
				sock.send(*msg);
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