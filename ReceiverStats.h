#ifndef DCENGINES_LISTENERSTATS_H
#define DCENGINES_LISTENERSTATS_H

#include <iostream>
#include <string>
#include <chrono>

namespace transport {

class ReceiverStats {
public:
	size_t intervalMessages{0};
	size_t totalMessages{0};
	ssize_t intervalVolumeIn{0};
	ssize_t totalVolumeIn{0};
	ssize_t VMAused{0};
	
	void incr(size_t msg_size) {
		intervalMessages++;
		totalMessages++;
		intervalVolumeIn += msg_size;
		totalVolumeIn += msg_size;
	}
	
	void sum(ReceiverStats *other) {
		intervalMessages += other->intervalMessages;
		totalMessages += other->totalMessages;
		intervalVolumeIn += other->intervalVolumeIn;
		totalVolumeIn += other->totalVolumeIn;
	}
	
	void print_summary(std::chrono::milliseconds interval, bool resetInterval = true) {
		//std::cout << "Thread " << thread_rank << " stats: " << std::endl;
		std::cout << "\tMessages: " << intervalMessages << ", Volume: " << intervalVolumeIn << "B, "	<< ", in " << std::chrono::duration_cast<std::chrono::seconds>(interval).count() << " seconds. ";
		std::cout << "Rate: " << intervalMessages*1000 / std::chrono::duration_cast<std::chrono::milliseconds>(interval).count()
			<< " msgs/sec, Throughput: " << intervalVolumeIn*1000 / std::chrono::duration_cast<std::chrono::milliseconds>(interval).count() << " Bps" << std::endl;
		std::cout << "\tTotal messages: " << totalMessages << " messages, Total Volume: " << totalVolumeIn << "B" << std::endl;
		if (resetInterval == true) {
			intervalMessages = 0;
			intervalVolumeIn = 0;
		}
		if (VMAused) {
			std::cout << "\tVMAused: " << VMAused << std::endl;
		}
	}
};

}
#endif