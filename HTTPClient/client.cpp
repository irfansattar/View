#include <iostream>
#include <string>
#include <WS2tcpip.h>		//The way Windows accesses network sockets

#pragma comment (lib, "ws2_32.lib")

void main()
{
	std::string ipAddress = "127.0.0.1";		//IP Address of server
	int port = 54000;				//Listening port of server

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

	do
	{
		//Promt user for text
		std::cout << "> ";
		std::getline(std::cin, userInput);

		if (userInput.size() > 0)
		{
			//Send text
			int sendResult = send(sock, userInput.c_str(), userInput.size() + 1, 0);

			if (sendResult != SOCKET_ERROR)
			{
				//Wait for response
				ZeroMemory(buf, 4096);
				int bytesReceived = recv(sock, buf, 4096, 0);
				if (bytesReceived > 0)
				{
					//Print response to user
					std::cout << "Server> " << std::string(buf, 0, bytesReceived) << std::endl;
				}
			}
		}
	} while (userInput.size() > 0);

	//CLose everything
	closesocket(sock);
	WSACleanup();
}