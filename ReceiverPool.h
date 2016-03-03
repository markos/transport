#ifndef DCENGINES_LISTENERPOOL_H
#define DCENGINES_LISTENERPOOL_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "Receiver.h"
#include "ReceiverStats.h"

namespace transport {

class ReceiverPool {
public:
	static constexpr std::chrono::milliseconds interval{2000};
	
	static ReceiverPool & Instance(Transport transport, const std::string &address, const size_t port, const size_t threads);
	
	size_t size();
	
	void register_stats(ReceiverStats *stats);
	void print_stats();
protected:
	std::vector<std::thread> receivers;
	std::vector<ReceiverStats *> receiverStats;
	
	// delete copy and move constructors and assign operators
	ReceiverPool(ReceiverPool const&) = delete;             // Copy constructor
	ReceiverPool(ReceiverPool &&) = delete;                 // Move constructor
	ReceiverPool& operator=(ReceiverPool const&) = delete;  // Copy assign
	ReceiverPool& operator=(ReceiverPool &&) = delete;      // Move assign

	// Default constructor
	ReceiverPool(Transport transport, const std::string &address, const size_t port, const size_t threads);

	~ReceiverPool();
};

}

#endif