#include <string>
#include <nnxx/message.h>

#include "TransmitterPool.h"
#include "Transmitter.h"

using namespace std;

namespace transport {

class ZeroMQTransmitter: public Transmitter {
public:
	ZeroMQTransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool);
	
	void operator()() override;
};

}