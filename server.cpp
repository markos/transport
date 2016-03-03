//#include <cassert>
#include <string>
#include <iostream>
#include <gflags/gflags.h>

#include "common.h"
#include "ReceiverPool.h"

using namespace std;
using namespace transport;

DEFINE_string(transport, "nanomsg", "can be one of nanomsg,zmq,vma");
DEFINE_string(address, "192.168.1.1", "run server on this IP address");
DEFINE_int32(port, 24242, "base port, Nth thread will use this port +N");
DEFINE_int32(threads, 1, "number of threads");

int main(int argc, char *argv[]) {
	::google::ParseCommandLineFlags(&argc, &argv, true);

	Transport transport = Transport::TRANSPORT_NANOMSG;
	if (FLAGS_transport == "nanomsg") {
		transport = Transport::TRANSPORT_NANOMSG;
	} else if (FLAGS_transport == "zmq") {
		transport = Transport::TRANSPORT_ZMQ;
	} else if (FLAGS_transport == "vma") {
		transport = Transport::TRANSPORT_VMA;
	} else if (FLAGS_transport == "rsockets") {
		transport = Transport::TRANSPORT_RDMACM;
	}
	
	std::string address{FLAGS_address};
	size_t port = FLAGS_port;
	size_t threads = FLAGS_threads;
	ReceiverPool &receiverPool = ReceiverPool::Instance(transport, address, port, threads);
	cout << "Started " << receiverPool.size() << " threads" << endl;
	
	return 0;
}
