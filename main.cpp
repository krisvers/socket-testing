#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <cstring>

int main(int argc, char** argv) {
	bool is_server;
	std::string ip = "127.0.0.1";
	int port = -1;

	if (argc == 2) {
		is_server = true;
		port = atoi(argv[1]);
		if (port == 0) {
			std::cout << "Invalid port " << argv[1] << '\n';
			return 1;
		}
	} else if (argc == 3) {
		is_server = false;
		ip = argv[1];
		port = atoi(argv[2]);
		if (port == 0) {
			std::cout << "Invalid port " << argv[2] << '\n';
			return 1;
		}
	} else {
		std::cout << "Give two arguments [ip] [port] to connect to server or one argument [port] to serve on said port\n";
		return 1;
	}

	unsigned short netport = htons(port);
	int sock = -1;
	sockaddr_in sockin;
	if (is_server) {
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == -1) {
			std::cout << "Failed to create socket\n";
			return 1;
		}

		memset(&sockin, 0, sizeof(sockin));
		sockin.sin_family = AF_INET;
		sockin.sin_port = netport;
		sockin.sin_addr.s_addr = htonl(INADDR_ANY);

		if (bind(sock, (sockaddr*) &sockin, sizeof(sockin)) == -1) {
			std::cout << "Failed to bind socket\n";
			close(sock);
			return 1;
		}

		if (listen(sock, 10) == -1) {
			std::cout << "Failed to listen\n";
			close(sock);
			return 1;
		}
		
		while (1) {
			int connect = accept(sock, nullptr, nullptr);
			if (connect == -1) {
				std::cout << "Failed to accept connection\n";
				close(sock);
				return 1;
			}

			std::cout << "success\n";

			if (shutdown(connect, SHUT_RDWR) == -1) {
				std::cout << "Failed to shutdown\n";
				close(connect);
				close(sock);
				return 1;
			}
			close(connect);
		}

		close(sock);
	} else {
		sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock == -1) {
			std::cout << "Failed to create socket\n";
			return 1;
		}

		memset(&sockin, 0, sizeof(sockin));
		sockin.sin_family = AF_INET;
		sockin.sin_port = netport;
		if (inet_pton(AF_INET, argv[1], &sockin.sin_addr) != 1) {
			std::cout << "Invalid address to connect to\n";
			close(sock);
			return 1;
		}

		if (connect(sock, (sockaddr*) &sockin, sizeof(sockin)) == -1) {
			std::cout << "Failed to connect to " << argv[1] << ":" << argv[2] << '\n';
			close(sock);
			return 1;
		}

		close(sock);
	}

	std::cout << "success\n";
	return 0;
}