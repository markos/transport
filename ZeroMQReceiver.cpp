#include <string>
#include <iostream>
#include <chrono>
#include <system_error>

#include <capnp/serialize.h>
#include <zmqpp/zmqpp.hpp>

#include "common.h"
#include "ReceiverStats.h"
#include "ZeroMQReceiver.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

template<typename T, typename M>
typename std::enable_if<std::is_same<M, zmqpp::message &>::value, T>::type
&deserialize(M &msg) {
	auto message_segments = kj::heapArray<kj::ArrayPtr<const capnp::word>>(msg.parts());

	for (size_t i=0; i < msg.parts(); i++) {
		cout << "size: " << msg.size(i) << endl;
		cout << "get: " << msg.get(i) << endl;
		cout << "raw: " << msg.raw_data(i) << endl;
		cout << "raw+size: " << std::hex << reinterpret_cast<ptrdiff_t>(msg.raw_data(i)) + msg.size(i) << std::dec << endl;
		message_segments[i] = kj::arrayPtr(reinterpret_cast<const capnp::word*>(msg.raw_data(i)), msg.size(i)/sizeof(capnp::word));
	}
	capnp::SegmentArrayMessageReader reader(message_segments);
	return std::move(reader.getRoot<T>());
}

ZeroMQReceiver::ZeroMQReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool)
:Receiver(i, address, port, pool) {
	cout << "Starting ZeroMQ Receiver on " << serverIP << ", port " << serverPort << endl;
}

void
ZeroMQReceiver::operator()() {
	parentPool.register_stats(&stats);
	try {
		zmqpp::context context;
		zmqpp::socket_type type = zmqpp::socket_type::pull;
		zmqpp::socket sock{context, type};
		
		cout << "Thread " << index << ": Binding to " << endPoint.str() << "..." << endl;
		sock.bind(endPoint.str());
		
		// receive the message
		cout << "Thread " << index << ": Receiving messages..." << endl;

		/*zmqpp::poller poller;
		poller.add(socket);*/
		zmqpp::message msg;

		while(true) {
			do {
				sock.receive(msg);
			} while(sock.has_more_parts());
			//sock.send(msg);
			
			//auto msgReq = deserialize<MessageRequest>(msg);
			//cout << msgReq << endl;
			
			stats.incr(msg.size(0));
		}
		cout << "Finished." << endl;
	} catch (const std::system_error &e) {
		std::cerr << e.what() << std::endl;
		return;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

}