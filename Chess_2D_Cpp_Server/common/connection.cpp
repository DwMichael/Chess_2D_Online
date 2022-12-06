#include "connection.h"
#include <iostream>

Connection::Connection(int sock_fd, sockaddr_in addr) : socket_fd(sock_fd), address(addr) {}

Connection::~Connection() {
    if (Connected()) close(socket_fd);
}

bool Connection::Connected() {
    return socket_fd > 0;
}

bool Connection::isHeaderSet() {
    return header_exists;
}

ssize_t Connection::Read(std::function<void(std::shared_ptr<Message>)> message_handler) {
    if (isHeaderSet())
        return ReadBody(message_handler);
    else
        return ReadHeader();
}

ssize_t Connection::ReadHeader() {
    if (header_exists) return -1;
    if (!msg) msg.reset(new Message());

    ssize_t read_bytes = read(socket_fd, ((std::byte*)&(msg->header)) + byte_offset, sizeof(Header) - byte_offset);
    byte_offset += read_bytes;
    if (byte_offset == sizeof(Header)) {
        header_exists = true;
        byte_offset = 0;
        msg->body.resize(msg->header.body_size);
        msg->connection_id = id;
    }
    return read_bytes;
}

ssize_t Connection::ReadBody(std::function<void(std::shared_ptr<Message>)> message_handler) {
    if (!header_exists) return -1;

    ssize_t read_bytes = read(socket_fd, msg->body.data() + byte_offset, msg->header.body_size - byte_offset);
    byte_offset += read_bytes;
    if (byte_offset == msg->header.body_size) {
        message_handler(std::move(msg));
        header_exists = false;
        byte_offset = 0;
    }
    return read_bytes;
}

ssize_t Connection::Send(void *data, const int &type_id, const int &size) {
    Header header{type_id, size};
    if (ssize_t rv = send(socket_fd, &header, sizeof(Header), 0) < 0) {
        return rv;
    }
    return send(socket_fd, data, header.body_size, 0);
}

ssize_t Connection::SendString(const std::string &data){
    return Send((void*)data.c_str(), 0, data.length()); // type_id 0 represents string
}