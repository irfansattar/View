#include <iostream>
#include <string>
#include <WS2tcpip.h>		//The way Windows accesses network sockets

#pragma comment (lib, "ws2_32.lib")

void main()
{
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
	SOCKET listening = socket(AF_INET, SOCK_STREAM, 0);
	if (listening == INVALID_SOCKET)
	{
		std::cerr << "Can't create socket. Exit!" << std::endl;
		return;
	}


	//Bind IP address and port to socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	bind(listening, (sockaddr*)&hint, sizeof(hint));

	//Tell winsock the socket is for listening
	listen(listening, SOMAXCONN);

	fd_set master;

	FD_ZERO(&master);

	FD_SET(listening, &master);

	while (true)
	{
		fd_set copy = master;

		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; i++)
		{
			SOCKET sock = copy.fd_array[i];

			if (sock == listening)
			{
				//Accept new connection
				SOCKET client = accept(listening, nullptr, nullptr);

				//Add new connection to list of connected clients
				FD_SET(client, &master);

				//Send welcome message to connected client
				std::string welcomeMsg = "Welcome to server.";
				send(client, welcomeMsg.c_str(), welcomeMsg.size() + 1, 0);
			}
			else
			{
				char buf[4096];
				ZeroMemory(buf, 4096);

				//Accept new message
				int bytesIn = recv(sock, buf, 4096, 0);
				if (bytesIn <= 0)
				{
					//Drop client
					closesocket(sock);
					FD_CLR(sock, &master);
				}
				else
				{
					//Send message to other clients, not listening socket
					for (int i = 0; i < master.fd_count; i++)
					{
						SOCKET	outSock = master.fd_array[i];
						if (outSock != listening && outSock != sock)
						{
							send(outSock, buf, 4096, 0);
						}
					}
				}
			}
		}
	}

	//Cleanup winsock
	WSACleanup();
}
