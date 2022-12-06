#include "server_interface.h"
#include <iostream>


ServerInterface::ServerInterface(int port) {
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
}

ServerInterface::~ServerInterface() {
    shutdown(master_socket_fd, SHUT_RDWR);
}

int ServerInterface::Initialize() {
    if ((master_socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }
    int opt = 1;
    if (setsockopt(master_socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return -1;
    }
    if (bind(master_socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return -1;
    }
    if (listen(master_socket_fd, 10) < 0) {
        perror("listen");
        return -1;
    }
    recieving_thread = std::thread(&ServerInterface::Read, this);

    return 0;
}

void ServerInterface::SendAll(std::string message) {
    std::scoped_lock lock(mux_conn);	
    for(auto c_itr = connections.begin(); c_itr != connections.end(); ) {
        ssize_t read_bytes = c_itr->second->SendString(message);
        if (read_bytes < 0)
            c_itr = connections.erase(c_itr);
        else
            ++c_itr;
    }
}

int ServerInterface::Read() {
    fd_set readfds; // File descriptor set.

    while(true) {
        FD_ZERO(&readfds); // Reset fd_set.
        FD_SET(master_socket_fd, &readfds); // Add master socket to fd_set (used for accepting new connections).

        int max_fd = master_socket_fd; // max_fd: File descriptor with highest file descriptor value.
        {
            std::scoped_lock lock(mux_conn);
            for (auto &c : connections)
                FD_SET(c.first, &readfds); // Adding fd of every connection to fd_set.
            if (!connections.empty() && (connections.rbegin())->first > max_fd)
                max_fd = (connections.rbegin())->first; // rbegin points to highest key (in this case file descriptor) since std::map is sorted.
        }

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);  // Waiting until any file descriptor becomes avialable (new action)
        if ((activity < 0) && (errno != EINTR)) {  
            // TODO: handle error 
        }

        std::scoped_lock lock(mux_conn);	

        if (FD_ISSET(master_socket_fd, &readfds)) { // Handling new connection.
            int addr_size = sizeof(address);
            int socket_fd = accept(master_socket_fd, (struct sockaddr*)&address, (socklen_t*)&addr_size);
            connections[socket_fd] = std::make_unique<Connection>(socket_fd, address);
        }  
        
        for(auto c_itr = connections.begin(); c_itr != connections.end(); ) {
            if (FD_ISSET(c_itr->first, &readfds)) { // Handling new data.

                ssize_t read_bytes = c_itr->second->Read([&](std::shared_ptr<Message> msg)->void{
                    messages.push_back(msg);
                });

                if (read_bytes == 0) {
                    c_itr = connections.erase(c_itr);
                    continue;
                }
            }
            ++c_itr;
        }
    }
}
