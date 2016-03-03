#ifndef DCENGINES_COMMON_H
#define DCENGINES_COMMON_H

#include <utility>

namespace transport {

using MessageItem = std::pair<void *, size_t>;

enum Transport
{
	TRANSPORT_ZMQ,
	TRANSPORT_NANOMSG,
	TRANSPORT_VMA,
	TRANSPORT_RDMACM
};

}

#endif
