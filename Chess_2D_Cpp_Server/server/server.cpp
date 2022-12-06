#include "datatypes.h"
#include "tsqueue.h"
#include "server_interface.h"


int main(int argc, char const* argv[])
{
	using namespace std::chrono_literals;

	ServerInterface server(5555);
	server.Initialize();

	ChessEngine chess{};

	// chess.tryMove("b2", "b4");
	chess.printBoard();

	while (true) {
		for(size_t i = 0; i < server.messages.count(); ++i){
			auto msg = server.messages.pop_front();
			std::string str(msg->body.begin(), msg->body.end());
			printf("MESSAGE FROM %i> %s\n", msg->connection_id, str.c_str());
			printf("%d\n", msg->header.type_id);
			printf("%d\n", msg->header.body_size);
			printf("%s\n", str.c_str());
			if (chess.player == Player::White && msg->connection_id != 1) {
				printf("Player with id %d tried to move on white's turn. (Only player with id 1 can do that)", msg->connection_id);
				continue;
			}
			if (chess.player == Player::Black && msg->connection_id != 2) {
				printf("Player with id %d tried to move on black's turn. (Only player with id 2 can do that)", msg->connection_id);
				continue;
			}

			if (msg->header.type_id == 1 && msg->header.body_size == 4){
				chess.printBoard();
				printf("Processing chess move\n");
				std::string from = str.substr(0, 2);
				std::string to = str.substr(2, 2);

				if (chess.tryMove(from, to)) {
				} else {
					printf("Invalid move!\n");
				}
			}
			
			
			
		}	
	}

	// std::this_thread::sleep_for(10s);
	// std::string msg = "Hello World!";
	// server.SendAll(msg);

	return 0;
}
