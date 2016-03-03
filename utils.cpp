#include <cstdint>
#include <iostream>
#include <algorithm>
#include <utility>

#include <capnp/serialize.h>

#include "message.capnp.h"
#include "utils.h"

using namespace std;

void
hexdump(void *ptr, size_t len) {
	uint8_t *buf = (uint8_t*) ptr;

	for (size_t i = 0; i < len; i += 16) {
		printf("%06lx: ", i);
		for (size_t j = 0; j < 16; j++) 
			if (i + j < len)
				printf("%02x ", buf[i + j]);
			else
				printf("   ");
		printf(" ");
		for (size_t j = 0; j < 16; j++) 
			if (i + j < len)
				printf("%c", isprint(buf[i + j]) ? buf[i + j] : '.');
		printf("\n");
	}
}

std::string
random_string( size_t length ) {
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

transport::MessageItem
makeRandomMessage(uint64_t id, size_t size) {
	// create a message
	capnp::MallocMessageBuilder parameterBuilder;
	parameterBuilder.initRoot<transport::Parameter>();
	auto param1 = parameterBuilder.getRoot<transport::Parameter>();
	param1.setKey("arg1");
	param1.setValue(random_string(size));
	capnp::MallocMessageBuilder messageBuilder;
	messageBuilder.initRoot<transport::MessageRequest>();
	auto msgreq = messageBuilder.getRoot<transport::MessageRequest>();
	msgreq.setId(id);
	msgreq.setCmd(transport::CommandType::HEARTBEAT);
	kj::Array<capnp::word> array = capnp::messageToFlatArray(messageBuilder);
	size_t data_size = (array.size() * sizeof(capnp::word));
	cout << "array.size() = " << array.size() << endl;
	cout << "data_size = " << data_size << endl;
	
	return make_pair(reinterpret_cast<void *>(array.begin()), data_size);
}