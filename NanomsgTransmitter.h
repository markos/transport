#include <string>
#include <nnxx/message.h>

#include "TransmitterPool.h"
#include "Transmitter.h"

using namespace std;

namespace transport {

class NanomsgTransmitter: public Transmitter {
public:
	NanomsgTransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool);
	
	void operator()() override;
};

}