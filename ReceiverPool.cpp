#include <string>
#include <iostream>
#include <chrono>
#include <thread>

#include "common.h"
#include "ReceiverPool.h"
#include "NanomsgReceiver.h"
#include "ZeroMQReceiver.h"
#include "VMAReceiver.h"
#include "RsocketsReceiver.h"

namespace transport {

// Default constructor
ReceiverPool::ReceiverPool(Transport transport, const std::string &address, const size_t port = 24242, const size_t threads = 1) {
	switch (transport) {
	case Transport::TRANSPORT_NANOMSG:
		cout << "starting " << threads << " Nanomsg server threads" << endl;
		for (size_t i=0; i< threads; i++) {
			receivers.emplace_back(std::thread(NanomsgReceiver(i, address, port +i, *this)));
		}
		break;
	case Transport::TRANSPORT_ZMQ:
		cout << "starting " << threads << " ZeroMQ server threads" << endl;
		for (size_t i=0; i< threads; i++) {
			receivers.emplace_back(std::thread(ZeroMQReceiver(i, address, port +i, *this)));
		}
		break;
	case Transport::TRANSPORT_VMA:
		cout << "starting " << threads << " VMA server threads" << endl;
		for (size_t i=0; i< threads; i++) {
			receivers.emplace_back(std::thread(VMAReceiver(i, address, port +i, *this)));
		}
		break;
	case Transport::TRANSPORT_RDMACM:
		cout << "starting " << threads << " Rsockets server threads" << endl;
		for (size_t i=0; i< threads; i++) {
			receivers.emplace_back(std::thread(RsocketsReceiver(i, address, port +i, *this)));
		}
		break;
	}
	
	cout << "Starting Statistic thread" << endl;
	std::thread([this]() {
        while (true)
        {
            print_stats();
            std::this_thread::sleep_for(interval);
        }
    }).detach();
}

ReceiverPool::~ReceiverPool() {
	for (auto &receiver: receivers)
		receiver.join();
}

ReceiverPool & 
ReceiverPool::Instance(Transport transport, const std::string &address, const size_t port, const size_t threads) {
	static ReceiverPool pool{transport, address, port, threads};
	
	return pool;
}
	
size_t 
ReceiverPool::size() {
	return receivers.size();
}

void
ReceiverPool::register_stats(ReceiverStats *stats) {
	receiverStats.emplace_back(stats);
}

void
ReceiverPool::print_stats() {
	ReceiverStats total_stats{};
	size_t i{0};
	for(ReceiverStats *stats: receiverStats) {
		cout << "Thread " << ++i << " stats: " << endl;
		total_stats.sum(stats);
		stats->print_summary(interval);
	}

	cout << "Total stats: " << endl;
	total_stats.print_summary(interval, false);
}

}