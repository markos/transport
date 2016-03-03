#include <string>

#include "ReceiverPool.h"
#include "Receiver.h"

using namespace std;

namespace transport {

class RsocketsReceiver: public Receiver {
public:
	static constexpr size_t BUFFER_SIZE{1 << 20};
	static constexpr size_t TIMEOUT_SEC{10};
	static constexpr size_t MAX_EVENTS{1000};

	RsocketsReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool);
	
	void operator()() override;
private:
	int sock{-1};
	int flags{0};
	sockaddr_in servAddr{};
	uint8_t buffer[BUFFER_SIZE];
};

}