
#include <sys/socket.h>
#include <arpa/inet.h>
#include <mellanox/vma_extra.h>

#include "Receiver.h"
#include "ReceiverPool.h"

using namespace std;

namespace transport {

class VMAReceiver: public Receiver {
public:
	static constexpr size_t BUFFER_SIZE{1 << 20};
	static constexpr size_t TIMEOUT_SEC{10};
	static constexpr size_t MAX_EVENTS{1000};

	VMAReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool);
	
	void operator()() override;
private:
	struct vma_api_t *vma_api{nullptr};
	int sock{-1};
	sockaddr_in servAddr{};
	uint8_t buffer[BUFFER_SIZE];
};

}