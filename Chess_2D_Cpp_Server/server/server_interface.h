#pragma once 

#include <arpa/inet.h> 
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <string.h>


#include "datatypes.h"
#include "tsqueue.h"
#include "connection.h"

#include "chess.h"


class ServerInterface {
	private:

	sockaddr_in address;
	int master_socket_fd;
	
	std::map<int, std::unique_ptr<Connection>> connections; // std::map<socket_fd, Connection>

	std::mutex mux_conn;

	public:
    
	std::thread recieving_thread;
	TSQueue<std::shared_ptr<Message>> messages;

	ServerInterface(int port);

	~ServerInterface();

	// Preparing master socket (must be ran first)
	int Initialize();

	// Send string to all connections
	void SendAll(std::string message);

	private:

	// Blocking read/accept loop
	int Read();

};