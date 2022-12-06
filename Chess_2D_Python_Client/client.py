import socket
import pickle
import time

class Network:
    def __init__(self):
        self.client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.host = "localhost" # change to server ip
        self.port = 5555
        self.addr = (self.host, self.port)
        self.connect()
        self.client.setblocking(False)

        self.received_data = bytearray()

    def connect(self):
        self.client.connect(self.addr)

    def disconnect(self):
        self.client.close()

    def send(self, data: str, pick=False):
        """
        :param data: str
        :return: str
        """
        header = int.to_bytes(1, 4, "little")  # first byte (type of message)
        header += int.to_bytes(len(data), 4, "little")  # second byte (length of message)
        self.client.send(header)
        self.client.send(bytearray(data, encoding="ASCII"))

    def read(self):
        messages = []

        while True:
            try:
                self.received_data += self.client.recv(4096 * 8)
            except BlockingIOError:
                # print("no more data")
                break

        while len(self.received_data) >= 8:
            message_type = int.from_bytes(self.received_data[0:4], "little")
            message_length = int.from_bytes(self.received_data[4:8], "little")
            if len(self.received_data) - 8 >= message_length:
                messages.append(self.received_data[8:message_length + 8].decode("ascii"))
                self.received_data = self.received_data[message_length + 8:]
            else:
                break

        return messages