#include <string>
#include <iostream>

#include <zmqpp/zmqpp.hpp>

#include "ReceiverPool.h"
#include "Receiver.h"

using namespace std;

namespace transport {

class ZeroMQReceiver: public Receiver {
public:
	ZeroMQReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool);
	
	void operator()() override;
};

}