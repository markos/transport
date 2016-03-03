#include <iostream>
#include <chrono>
#include <thread>

#include "common.h"
#include "TransmitterPool.h"
#include "NanomsgTransmitter.h"
#include "ZeroMQTransmitter.h"
#include "VMATransmitter.h"
#include "RsocketsTransmitter.h"

namespace transport {

// Default constructor
TransmitterPool::TransmitterPool(Transport transport, const std::string &address, const size_t port = 24242, const size_t threads = 1) {
	switch (transport) {
	case Transport::TRANSPORT_NANOMSG:
		cout << "starting " << threads << " Nanomsg transmitter threads" << endl;
		for (size_t i=0; i< threads; i++) {
			transmitters.emplace_back(std::thread(NanomsgTransmitter(i, address, port +i, *this)));
		}
		break;
	case Transport::TRANSPORT_ZMQ:
		cout << "starting " << threads << " ZeroMQ transmitter threads" << endl;
		for (size_t i=0; i< threads; i++) {
			transmitters.emplace_back(std::thread(ZeroMQTransmitter(i, address, port +i, *this)));
		}
		break;
	case Transport::TRANSPORT_VMA:
		cout << "starting " << threads << " VMA transmitter threads" << endl;
		for (size_t i=0; i< threads; i++) {
			transmitters.emplace_back(std::thread(VMATransmitter(i, address, port +i, *this)));
		}
		break;
	case Transport::TRANSPORT_RDMACM:
		cout << "starting " << threads << " Rsockets transmitter threads" << endl;
		for (size_t i=0; i< threads; i++) {
			transmitters.emplace_back(std::thread(RsocketsTransmitter(i, address, port +i, *this)));
		}
		break;
	}
	
	/*cout << "Starting Statistic thread" << endl;
	std::thread([this]() {
        while (true)
        {
            print_stats();
            std::this_thread::sleep_for(interval);
        }
    }).detach();*/
}

TransmitterPool::~TransmitterPool() {
	for (auto &transmitter: transmitters)
		transmitter.join();
}

TransmitterPool & 
TransmitterPool::Instance(Transport transport, const std::string &address, const size_t port, const size_t threads) {
	static TransmitterPool pool{transport, address, port, threads};
	
	return pool;
}
	
size_t 
TransmitterPool::size() {
	return transmitters.size();
}

void
TransmitterPool::send(MessageItem &msg) {
	msgQueue.enqueue(msg);
}

void
TransmitterPool::shutdown() {
	msgQueue.enqueue(std::make_pair<void *, size_t>(nullptr, 0));
}
}