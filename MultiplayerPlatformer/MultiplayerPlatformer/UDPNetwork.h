//
//  UDPNetwork.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 01/12/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

#include <stdio.h>
#include <string.h>

#ifdef __APPLE__

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

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

	struct sockaddr_in si_other_;
	int socket_, slen_ = sizeof(si_other_);

	char recvBuff_[1024];

};

