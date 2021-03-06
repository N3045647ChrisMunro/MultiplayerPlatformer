//
//  TCPNetwork.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 10/11/2016.
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
#include "GameDataTCP.pb.h"

class TCPNetwork{

public:
    
    TCPNetwork();
    ~TCPNetwork();
    
    void createSocket();
    bool connectToServer();
	GameDataTCP::DataMessage* receiveData();
    void sendData(std::string message);
    
	void setIP_address(std::string ip);
	void setPortNumber(std::string port);
    void setPortNumber(int port);


	bool isConnected() const;
    
private:
    
    GameDataTCP::DataMessage *dataMsg_{ nullptr };
    
    int sockfd_ = 0;
    
    char recvBuff_[1024];

	std::string ip_;
	std::string port_;
    
#ifdef __APPLE__

    struct sockaddr_in serv_addr_;
    int myPort_;

#elif _WIN32

	struct addrinfo *local_ = NULL,
					*ptr_ = NULL,
					hints_;

	SOCKET ConnectSocket_ = INVALID_SOCKET;
	int myPort_;

#endif
    
};

