#pragma once

#include <vector>

// Static size header sent with every message.
struct Header {
	int type_id; // Id that defines how body of the message should be interpreted.
	int body_size; // Size of the body of the message in bytes.
};

// Struct for holding message data. Do not confuse with data that is being received from socket (only header and body is read from the socket)
struct Message {
	Header header;
	int connection_id;
	std::vector<char> body;
};