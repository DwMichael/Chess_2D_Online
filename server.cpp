#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string>
#include <thread>
#include <sys/time.h>
#include <arpa/inet.h> 
#include <iostream>
#include <chrono>
#include <deque>
#include <mutex>
#include <vector>
//#All rights reserved
// #define LOG(x) (std::cout << (x) << std::endl)
#define LOG(x)


template<typename T>
class TSQueue {
protected:
	std::mutex muxQueue;
	std::deque<T> deQueue;
public:
	TSQueue() = default;
	TSQueue(const TSQueue<T>&) = delete;

	const T& front() {
		std::scoped_lock lock(muxQueue);
		return deQueue.front();
	}

	const T& back() {
		std::scoped_lock lock(muxQueue);
		return deQueue.back();
	}

	void push_back(const T& item) {
		std::scoped_lock lock(muxQueue);
		deQueue.emplace_back(std::move(item));
	}

	void push_front(const T& item) {
		std::scoped_lock lock(muxQueue);
		deQueue.emplace_front(std::move(item));
	}

	T pop_back() {
		std::scoped_lock lock(muxQueue);
		T tmp = std::move(deQueue.back());
		deQueue.pop_back();
		return tmp;
	}

	T pop_front() {
		std::scoped_lock lock(muxQueue);
		T tmp = std::move(deQueue.front());
		deQueue.pop_front();
		return tmp;
	}

	size_t count() {
		std::scoped_lock lock(muxQueue);
		return deQueue.size();
	}

	void clear() {
		std::scoped_lock lock(muxQueue);
		deQueue.clear();
	}

};

struct Message {
	int connection_id;
	std::string body;
};

class ServerInterface {
	private:

	struct sockaddr_in address;
	int master_socket;
	int max_clients;
	int addrlen;
	
	std::vector<int> client_socket;

	std::mutex muxConn;

	public:
	std::thread recieve_thread;
	TSQueue<Message> messages;

	ServerInterface(int port, int max_c) : max_clients(max_c) {
		client_socket.resize(max_c);
		for (int i = 0; i < max_clients; ++i)
			client_socket[i] = 0;

		memset(&address, 0, sizeof(address));
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(port);

		addrlen = sizeof(address);
	}

	~ServerInterface(){
		shutdown(master_socket, SHUT_RDWR);
	}

	int Initialize() {

		if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
			perror("socket failed");
			return -1;
		}
		int opt = 1;
		if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
			perror("setsockopt");
			return -1;
		}
		if (bind(master_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
			perror("bind failed");
			return -1;
		}
		if (listen(master_socket, 10) < 0) {
			perror("listen");
			return -1;
		}
		recieve_thread = std::thread(&ServerInterface::Read, this);

		return 0;
	}

	int Read(){
		fd_set readfds;
		char buffer[1024];

		while(true) {

			FD_ZERO(&readfds);
			FD_SET(master_socket, &readfds);
	void WriteAll(const char* message, size_t size){
		std::scoped_lock lock(muxConn);	
		for(int i = 0; i < max_clients; ++i){
			if (client_socket[i] > 0){
				LOG("Sending message to all!");
				send(client_socket[i], message, size, 0);
			}
		}
	}

			int max_sd = master_socket;  
				
			for (int i = 0; i < max_clients; ++i)  
			{  
				std::scoped_lock lock(muxConn);
				int sd = client_socket[i];  
				if(sd > 0)  
					FD_SET(sd, &readfds);  
					
				if(sd > max_sd)  
					max_sd = sd;
			}

			LOG("Waiting for action");
			int activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);  
			LOG("New action.");

			if ((activity < 0) && (errno != EINTR)) {  
				LOG("Select error");
			}

			std::scoped_lock lock(muxConn);	

			if (FD_ISSET(master_socket, &readfds)) {  
				int new_socket;
				if ((new_socket = accept(master_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {  
					perror("accept");  
					exit(EXIT_FAILURE);  
				}  
				printf("New connection , socket fd is %d , ip is : %s , port : %d\n", new_socket, inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
				
				for (int i = 0; i < max_clients; i++)  
				{  
					
					if(client_socket[i] == 0) {  
						client_socket[i] = new_socket;  
						printf("Adding to list of sockets as %d\n", i);  
						break;  
					}  
				}  
			}  
				
			for (int i = 0; i < max_clients; i++) {  
				int sd = client_socket[i];  
					
				if (FD_ISSET(sd, &readfds)) {  
					int valread;
					if ((valread = read(sd, buffer, 1024)) == 0) {  
						LOG("Connection closed");
						close(sd);  
						client_socket[i] = 0;  
					} else {  
						LOG("Message received");
						Message msg;
						msg.connection_id = i;
						msg.body = std::string(buffer);
						messages.push_back(msg);
					}  
				}  
			}
		}
		
	}

	void WriteAll(const char* message, size_t size){
		std::scoped_lock lock(muxConn);	
		for(int i = 0; i < max_clients; ++i){
			if (client_socket[i] > 0){
				LOG("Sending message to all!");
				send(client_socket[i], message, size, 0);
			}
		}
	}


};


int main(int argc, char const* argv[])
{
	using namespace std::chrono_literals;
	LOG("START");
	ServerInterface server(7778, 20);
	
	std::cout << server.Initialize() << std::endl;
	while (true) {
		for(size_t i = 0; i < server.messages.count(); ++i){
			auto msg = server.messages.pop_front();
			printf("MESSAGE FROM %i> %s\n", msg.connection_id, msg.body.c_str());
		}

	}
	std::this_thread::sleep_for(10s);
	// std::string msg = "Hello World!";
	// server.WriteAll(msg.c_str(), msg.length());

	return 0;
}
