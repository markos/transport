#include <cassert>
#include <string>
#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <system_error>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <mellanox/vma_extra.h>

#include "common.h"
#include "utils.h"
#include "VMAReceiver.h"

using namespace std;
namespace transport {

struct __attribute__ ((packed)) pending_packet_t {
	int                   valid;
	int                   iovec_size;
	struct iovec          iov[10];
	struct vma_info_t     *vma_info;
};
	
typedef vma_recv_callback_retval_t (*vma_recv_callback_t)(int fd, size_t sz_iov, struct iovec *iov,
                                                          struct vma_info_t* vma_info, void *context);
vma_recv_callback_retval_t myapp_vma_recv_pkt_notify_callback(int fd, size_t iov_sz, struct iovec *iov,
                                                              struct vma_info_t* vma_info, void *context)
{
  
	// Return zero copied packet buffer back to VMA
	// Would be better to collect a bunch of buffers and return them all at once
	// which will save locks inside VMA
	
	//free_packet(packet_id, s);
	return VMA_PACKET_HOLD;
}

VMAReceiver::VMAReceiver(const size_t i, const std::string &address, const size_t port, ReceiverPool &pool)
: Receiver(i, address, port, pool) {
	cout << "Starting VMA Receiver on " << serverIP << ", port " << serverPort << endl;

	int rc;
	struct pending_packet_t pending_packet;

	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(serverIP.c_str());
	servAddr.sin_port = htons(port +i);
		
	cout << "Thread " << index << ": Binding to " << endPoint.str() << "..." << endl;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	assert(sock);

	rc = bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));
	assert(rc == 0);

	rc = listen(sock,1);
	assert(rc == 0);

	vma_api = vma_get_api();
	assert(vma_api);

	cout << "VMA API available" << endl;

	rc = vma_api->register_recv_callback(sock, myapp_vma_recv_pkt_notify_callback, &pending_packet);
	cout << "Callback function registered with VMA\n" << endl;
}
	
void
VMAReceiver::operator()() {
	parentPool.register_stats(&stats);
	try {
		int received;
		int new_sock;
		int flags = MSG_VMA_ZCOPY_FORCE;
		struct sockaddr_in client_addr;
		unsigned int client_addr_len;
		
		new_sock = accept(sock, (struct sockaddr *) &client_addr, &client_addr_len);
		assert(new_sock);

		// receive the message
		cout << "Thread " << index << ": Receiving messages..." << endl;

		int epollfd = epoll_create1(0);
		assert(epollfd != -1);
		
		struct epoll_event ev{0}, events[MAX_EVENTS];
		
		ev.events = EPOLLIN;
		ev.data.fd = new_sock;

		int rc = epoll_ctl(epollfd, EPOLL_CTL_ADD, new_sock, &ev);
		assert(rc != -1);

		while(true) {
			int nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
			assert(nfds != -1);

			for (int n = 0; n < nfds; ++n) {
				//cout << "received " << nfds << " events" << endl;
				if (events[n].events & EPOLLIN) {
					received = vma_api->recvfrom_zcopy(events[n].data.fd, buffer, BUFFER_SIZE, &flags, NULL, NULL);
					if (received) {
						cout << "received " << received << " bytes" << endl;
						if (flags & MSG_VMA_ZCOPY) {
							vma_packets_t* vma_packets = (vma_packets_t*)buffer;
							cout << "vma_packets " << vma_packets->n_packet_num << endl;
							for (size_t i = 0; i < vma_packets->n_packet_num; i++) {
								vma_packet_t* vma_packet = &vma_packets->pkts[i];
								cout << "vma_packet[" << i << "] has " << vma_packet->sz_iov << " fragments "<< endl;
								for (size_t j = 0; j < vma_packet->sz_iov; j++) {
									cout << "vma_packet[" << i << "], fragment " << j << ":" << endl;
									//hexdump(vma_packet->iov[j].iov_base, vma_packet->iov[j].iov_len);
									stats.incr(vma_packet->iov[j].iov_len);
									stats.VMAused++;
									//auto msgReq = deserialize<MessageRequest>(msg);
									//cout << msgReq << endl;
								}
								//vma_api->free_packets(sock, vma_packet, vma_packet->sz_iov);
							}
							vma_api->free_packets(sock, vma_packets->pkts, vma_packets->n_packet_num);
						}
						//hexdump(buffer, received);
					}
				}
			}
		}
		cout << "Finished." << endl;
	} catch (const std::system_error &e) {
		std::cerr << e.what() << std::endl;
		return;
	}
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}

}