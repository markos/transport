CXX = g++-5
CXXFLAGS = -Wall -Werror -O2 -g -pthread -std=c++11
server_TARGET = server
client_TARGET = client
TARGETS = $(server_TARGET) $(zmq_client_TARGET) $(client_TARGET)
LDFLAGS = -pthread -std=c++11 -L/usr/local/lib 
MISC_LIBS = -lgflags
VMA_LIBS = #-lvma
CAPNP_LIBS = -lcapnp -lkj
ZMQ_LIBS = -lzmq -lzmqpp
NANOMSG_LIBS = -lnnxx -lnanomsg
RSOCKETS_LIBS = -lrdmacm

.PHONY: default all clean

default: $(TARGETS)
all: default

server_OBJECTS = server.o \
                 ReceiverPool.o \
                 NanomsgReceiver.o \
                 ZeroMQReceiver.o \
                 VMAReceiver.o \
		 RsocketsReceiver.o \
                 utils.o
client_OBJECTS = client.o \
                 TransmitterPool.o \
                 NanomsgTransmitter.o \
                 ZeroMQTransmitter.o \
                 VMATransmitter.o \
		 RsocketsTransmitter.o \
                 utils.o 

capnp:
	capnp compile -oc++ message.capnp

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(server_TARGET): $(server_OBJECTS)
	$(CXX) $(LDFLAGS) $(server_OBJECTS) $(ZMQ_LIBS) $(NANOMSG_LIBS) $(CAPNP_LIBS) $(MISC_LIBS) $(VMA_LIBS) $(RSOCKETS_LIBS) -o $@

$(client_TARGET): $(client_OBJECTS)
	$(CXX) $(LDFLAGS) $(client_OBJECTS) $(ZMQ_LIBS) $(NANOMSG_LIBS) $(CAPNP_LIBS) $(MISC_LIBS) $(RSOCKETS_LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f $(TARGETS)
