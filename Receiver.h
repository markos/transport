#ifndef DCENGINES_LISTENER_H
#define DCENGINES_LISTENER_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "ReceiverStats.h"

namespace transport {

class ReceiverPool;

template<typename T, typename M> T &deserialize(M &msg);

class Receiver {
public:
	Receiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool)
	: index(i), serverIP(address), serverPort(port), parentPool(pool) {
		endPoint << "tcp://";
		endPoint << serverIP;
		endPoint << ":";
		endPoint << serverPort;
	}
	
	virtual void operator()() = 0;
	
	ReceiverStats stats;
	const size_t index;
protected:
	const std::string serverIP;
	const size_t serverPort;
	ReceiverPool &parentPool;
	std::stringstream endPoint;
};

}
#endif