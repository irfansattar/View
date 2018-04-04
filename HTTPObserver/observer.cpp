#include <iostream>
#include <string>
#include <WS2tcpip.h>		//The way Windows accesses network sockets

#pragma comment (lib, "ws2_32.lib")


void main(int argc, char* argv[])
{
	
	if (argc == 1)
	{
		std::cout << "Usage: [Bulb Status: On, Off]" << std::endl;
		return;
	}

	std::string ipAddress = "127.0.0.1";		//IP Address of server to connect to server
	int port = 54000;				//Listening port of server to assess correct service within server

									//Initialize winsock
	WSADATA wsData;
	WORD ver = MAKEWORD(2, 2);

	int wsOk = WSAStartup(ver, &wsData);

	if (wsOk != 0)
	{
		std::cerr << "Can't initalize winsock. Exit!" << std::endl;
		return;
	}

	//Create socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket. Exit!" << std::endl;
		WSACleanup();
		return;
	}

	//Fill in a hint structure
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);

	//Connect to server
	int connResult = connect(sock, (sockaddr*)&hint, sizeof(hint));
	if (connResult == SOCKET_ERROR)
	{
		std::cerr << "Can't connect to server." << std::endl;
		closesocket(sock);
		WSACleanup();
		return;
	}

	//Loop to send/receive data
	char buf[4096];
	std::string userInput;

	int bytesReceived = recv(sock, buf, 4096, 0);

	std::cout << "Server> " << std::string(buf, 0, bytesReceived) << std::endl;

	while (true)
	{
		//Wait for response
		ZeroMemory(buf, 4096);
		int bytesReceived = recv(sock, buf, 4096, 0);
		if (std::string(buf, 0, bytesReceived) == "Status")
		{
			//Print server request
			std::cout << "Server> " << std::string(buf, 0, bytesReceived) << std::endl;
			std::string status = argv[1];
			std::cout << "Sending> " << status << std::endl;
			send(sock, status.c_str(), status.size() + 1, 0);
		}
		else
		{
			std::string error = "Incorrect input.";
			std::cout << "Sending> " << error << std::endl;
			send(sock, error.c_str(), error.size() + 1, 0);
		}
	}

	//CLose everything
	closesocket(sock);
	WSACleanup();
}