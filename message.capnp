@0xb1ed434000ad8669;

using Cxx = import "/capnp/c++.capnp";
$Cxx.namespace("transport");

enum CommandType {
	heartbeat @0;
	createCF @1;
	deleteCF @2;
	createCFIndex @3;
	deleteCFIndex @4;
	lookupOneCF @5;
	openCFCursor @6;
	nextCFCursor @7;
	closeCFCursor @8;
	bulkLoad @9;
}

enum StatusId {
    ok @0;
    error @1;
}

struct StatusMessage {
	status @0: StatusId;
	message @1: Text;
}

enum MessageResponseType {
    sync @0;
    async @1;
}

struct Parameter {
	key @0     : Text;
	value @1   : Text;
}

struct Record {
	key @0     : Data;
	value @1   : Data;
}

struct MessageRequest {
	id @0 : UInt64;
	cmd @1 : CommandType;
	type @3 : MessageResponseType;
	args @2 : List(Parameter);
}

struct MessageResponse {
	id @0 : UInt64;
	status @1 : StatusMessage;
	result @2 : List(Record);
}

struct MessageResponseList {
	responses @0 : List(MessageResponse);
}

