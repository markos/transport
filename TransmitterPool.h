#ifndef DCENGINES_CLIENTPOOL_H
#define DCENGINES_CLIENTPOOL_H

#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <utility>

#include "blockingconcurrentqueue.h"

#include "common.h"
#include "utils.h"
#include "Transmitter.h"

namespace transport {

class TransmitterPool {
public:
	static constexpr std::chrono::milliseconds interval{2000};
	
	static TransmitterPool & Instance(Transport transport, const std::string &address, const size_t port, const size_t threads);
	
	size_t size();
	
	void send(MessageItem &msg);
	void shutdown();
	
	moodycamel::BlockingConcurrentQueue<MessageItem> msgQueue;
protected:
	std::vector<std::thread> transmitters;
	
	// delete copy and move constructors and assign operators
	TransmitterPool(TransmitterPool const&) = delete;             // Copy constructor
	TransmitterPool(TransmitterPool &&) = delete;                 // Move constructor
	TransmitterPool& operator=(TransmitterPool const&) = delete;  // Copy assign
	TransmitterPool& operator=(TransmitterPool &&) = delete;      // Move assign

	// Default constructor
	TransmitterPool(Transport transport, const std::string &address, const size_t port, const size_t threads);

	~TransmitterPool();
};

}

#endif