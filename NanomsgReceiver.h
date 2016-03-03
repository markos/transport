#include <string>

#include "ReceiverPool.h"
#include "Receiver.h"

using namespace std;

namespace transport {

class NanomsgReceiver: public Receiver {
public:
	NanomsgReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool);
	
	void operator()() override;
};

}