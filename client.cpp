#include <string>
#include <iostream>
#include <chrono>
#include <utility>

#include <gflags/gflags.h>

#include "TransmitterPool.h"
#include "utils.h"

using namespace std;
using namespace transport;

DEFINE_string(transport, "nanomsg", "can be one of nanomsg,zmq,vma");
DEFINE_string(address, "192.168.1.1", "run server on this IP address");
DEFINE_int32(port, 24242, "base port, Nth thread will use this port +N");
DEFINE_int32(threads, 1, "number of threads");
DEFINE_uint64(messages, 10000000, "number of messages to send");

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
	
	TransmitterPool &transmitterPool = TransmitterPool::Instance(transport, FLAGS_address, FLAGS_port, FLAGS_threads);
	cout << "Started " << transmitterPool.size() << " threads" << endl;
	
	size_t size = 300;
	vector<MessageItem> messages;
	for (uint64_t i=0; i < FLAGS_messages; i++) {
		auto rawMsg = makeRandomMessage(i, size);
		hexdump(rawMsg.first, rawMsg.second);
		messages.emplace_back(rawMsg);
	}
	
	
	auto all_begin = std::chrono::high_resolution_clock::now();
	for (size_t i=0; i < FLAGS_messages; i++) {
		transmitterPool.send(messages[i]);
	}
	auto all_end = std::chrono::high_resolution_clock::now();
	std::chrono::nanoseconds total_time = all_end-all_begin;
	std::cout << "usec : " << std::chrono::duration_cast<std::chrono::microseconds>(total_time).count() << std::endl;
	std::cout << "cmds/sec : " << FLAGS_messages*1000000 / std::chrono::duration_cast<std::chrono::microseconds>(total_time).count() << std::endl;
	
	transmitterPool.shutdown();
}
