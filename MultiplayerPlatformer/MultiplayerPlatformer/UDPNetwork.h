//
//  UDPNetwork.hpp
//  PlatformerGame
//
//  Created by MUNRO, CHRISTOPHER on 02/12/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__

#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#elif _WIN32

#include <WinSock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#endif

#include <iostream>

class UDPNetwork
{
public:
	UDPNetwork();
	~UDPNetwork();

	void createSocket();
	void receiveData();
	void sendData(std::string message);

	void setIP_address(std::string ip);
	void setPortNumber(int port);

private:

	std::string ip_;
	int port_;

	char recvBuff_[1024];

#ifdef __APPLE__
	struct sockaddr_in ser_addr_;
	struct hostent *server_;
	int socket_, serlen_ = sizeof(ser_addr_), recv_len;

	char *hostname_;

#elif _WIN32

	struct sockaddr_in recvAddr_, senderAddr_;
	SOCKET socket_;

#endif // __APPLE__

};


