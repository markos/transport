#ifndef DCENGINES_TRANSMITTER_H
#define DCENGINES_TRANSMITTER_H

#include <iostream>
#include <sstream>
#include <string>

namespace transport {

class TransmitterPool;

template<typename T, typename M> T &serialize(M &msg);

class Transmitter {
public:
	Transmitter(const size_t i, const std::string address, const size_t port, TransmitterPool &pool)
	: index(i), serverIP(address),  serverPort(port), parentPool(pool) {
		endPoint << "tcp://";
		endPoint << serverIP;
		endPoint << ":";
		endPoint << serverPort;
	}
	
	virtual void operator()() = 0;
	
	const size_t index;
protected:
	const std::string serverIP;
	const size_t serverPort;
	TransmitterPool &parentPool;
	std::stringstream endPoint;
};

}
#endif