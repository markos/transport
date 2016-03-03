#include <string>
#include <memory>
#include <thread>
#include <queue>
#include <iostream>
#include <system_error>

#include <nnxx/message.h>
#include <nnxx/pipeline.h>
#include <nnxx/socket.h>

#include <capnp/serialize.h>

#include "utils.h"
#include "NanomsgTransmitter.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

NanomsgTransmitter::NanomsgTransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool)
: Transmitter(i, address, port, pool) {
	cout << "Connecting Nanomsg Transmitter to " << serverIP << ", port " << serverPort << endl;
}

void
NanomsgTransmitter::operator()() {
	try {
		nnxx::socket sock{ nnxx::SP, nnxx::PUSH };
		
		cout << "Thread " << index << ": Connecting to " << endPoint.str() << "..." << endl;
		sock.connect(endPoint.str());
		
		// send the messages
		cout << "Thread " << index << ": Sending messages..." << endl;

		while(true) {
			MessageItem rawMsg;
			parentPool.msgQueue.wait_dequeue(rawMsg);
			if (rawMsg.first != nullptr && rawMsg.second != 0) {
				cout << "rawMsg:" << rawMsg.first << ", " << rawMsg.second << endl;
				nnxx::message msg = nnxx::make_message_from(reinterpret_cast<nnxx::message::pointer>(rawMsg.first), rawMsg.second);
				sock.send(msg);
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