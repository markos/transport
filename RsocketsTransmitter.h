#include <string>

#include <arpa/inet.h>

#include "TransmitterPool.h"
#include "Transmitter.h"

using namespace std;

namespace transport {

class RsocketsTransmitter: public Transmitter {
public:
	static constexpr size_t BUFFER_SIZE = 1 << 20;
	static constexpr size_t TIMEOUT_SEC = 10;
	
	RsocketsTransmitter(const size_t i, const std::string &address, const size_t port, TransmitterPool &pool);
	
	void operator()() override;
private:
	int sock{-1};
	sockaddr_in servAddr{};
};

}