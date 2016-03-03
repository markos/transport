#include <string>
#include <memory>
#include <thread>
#include <iostream>
#include <system_error>

#include <nnxx/message.h>
#include <nnxx/pipeline.h>
#include <nnxx/socket.h>

#include <capnp/serialize.h>

#include "common.h"
#include "utils.h"
#include "ReceiverStats.h"
#include "NanomsgReceiver.h"
#include "message.capnp.h"

using namespace std;
namespace transport {

template<typename T, typename M>
typename std::enable_if<std::is_same<M, nnxx::message &>::value, T>::type
&deserialize(M &msg) {
	cout << "size: " << msg.size() << endl;
	auto message_array = kj::arrayPtr(reinterpret_cast<const capnp::word*>(msg.data()), msg.size()/sizeof(capnp::word));
	capnp::FlatArrayMessageReader reader(message_array);
	return std::move(reader.getRoot<T>());
}

NanomsgReceiver::NanomsgReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool)
: Receiver(i, address, port, pool) {
	cout << "Starting Nanomsg Receiver on " << serverIP << ", port " << serverPort << endl;
}

void
NanomsgReceiver::operator()() {
	parentPool.register_stats(&stats);
	try {
		nnxx::socket sock{ nnxx::SP, nnxx::PULL };

		cout << "Thread " << index << ": Binding to " << serverIP << "..." << endl;
		sock.bind(endPoint.str());
		
		// receive the message
		cout << "Thread " << index << ": Receiving messages..." << endl;

		while(true) {
			nnxx::message msg = sock.recv();
			//socket.send(msg);

			//auto msgReq = deserialize<MessageRequest>(msg);
			//cout << msgReq << endl;
				
			stats.incr(msg.size());
		}
		cout << "Finished." << endl;
	} catch (const std::system_error &e) {
		std::cerr << e.what() << std::endl;
		return;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

}