#pragma once

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h> 
#include <memory>
#include <functional>
#include <string>


#include "datatypes.h"

// Holds socket file descriptor and handles communication
class Connection {
	private:

	std::shared_ptr<Message> msg = nullptr;
	bool header_exists = false; // Set true when whole header read. Set false when whole message is received (preparation for next header).
	int byte_offset = 0; // Offset of body/header (used when data is read).

	static inline int next_id = 0;
	int id = ++next_id;

	public:

	int socket_fd;
	sockaddr_in address;
	
	// Create connection
	Connection() = default;

	Connection(int sock_fd, sockaddr_in addr);

	~Connection();

	bool Connected();
	bool isHeaderSet();

	// Reads data to either header or body. Requires function for processing new messages.
	ssize_t Read(std::function<void(std::shared_ptr<Message>)> message_handler);

	// Returns number of bytes read. Blocking if no header to be read.
	ssize_t ReadHeader();

	// Returns number of bytes read. Blocking if no body to be read. Requires function for processing new messages.
	ssize_t ReadBody(std::function<void(std::shared_ptr<Message>)> message_handler);

	// Send data of custom type

	ssize_t Send(void *data, const int &type_id, const int &size=0);

	// Send string.
	ssize_t SendString(const std::string &data);

};