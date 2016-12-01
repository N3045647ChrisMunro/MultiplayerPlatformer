//
//  TCPNetwork.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 10/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#include "TCPNetwork.hpp"
#include <string>

TCPNetwork::TCPNetwork()
{
    memset(recvBuff_, '0', sizeof(recvBuff_));

#ifdef __APPLE__

    memset(&serv_addr_, '0', sizeof(serv_addr_));

#elif _WIN32

	WSAData wsaData;

	sockfd_ = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (sockfd_ != 0) {
		std::cerr << "Error: WSAStartup Failed: " << sockfd_ << std::endl;
		return;
	}

#endif
}

TCPNetwork::~TCPNetwork()
{
#ifdef  __APPLE__


#elif _WIN32

	closesocket(ConnectSocket_);
	WSACleanup();

#endif //  __APPLE__
}

void TCPNetwork::createSocket()
{

#ifdef  __APPLE__

	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd_ < 0)
		std::cerr << "Error: Failed to Create Socket" << std::endl;

#elif _WIN32

	ZeroMemory(&hints_, sizeof(hints_));

	hints_.ai_family = AF_UNSPEC;
	hints_.ai_socktype = SOCK_STREAM;
	hints_.ai_protocol = IPPROTO_TCP;

	sockfd_ = getaddrinfo(NULL, port_.c_str(), &hints_, &local_);
	if (sockfd_ != 0) {
		std::cerr << "Error: Getaddrinfo Failed: " << sockfd_ << std::endl;
		WSACleanup();
	}

	// Attempt to connect to the first address returned by
	// the call to getaddrinfo
	ptr_ = local_;

	//Create a SOCKET for connecting to server
	ConnectSocket_ = socket(ptr_->ai_family, ptr_->ai_socktype, ptr_->ai_protocol);

	if (ConnectSocket_ == INVALID_SOCKET) {
		std::cerr << "Error: Invalid Socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
	}
	
#endif //  __APPLE__
}

void TCPNetwork::connectToServer()
{
#ifdef  __APPLE__
    serv_addr_.sin_family = AF_INET;
    serv_addr_.sin_port = htons(port);
    
    if(inet_pton(AF_INET, ip_.c_str(), &serv_addr_.sin_addr) <= 0)
        std::cerr << "Error: inet_pton Error Occured" << std::endl;
    
    if(connect(sockfd_, (struct sockaddr*)&serv_addr_, sizeof(serv_addr_)) < 0)
        std::cerr << "Error: Failed to Connect" << std::endl;

#elif _WIN32

	sockfd_ = getaddrinfo(ip_.c_str(), port_.c_str(), &hints_, &local_);

	if (sockfd_ != 0) {
		std::cerr << "Error: getaddrinfo failed with error: " << sockfd_ << std::endl;
		WSACleanup();
	}

	//Attempt to connect to an address until one succeeds
	for (ptr_ = local_; ptr_ != NULL; ptr_ = ptr_->ai_next) {

		//Create a SOCKET for connecting to the server
		ConnectSocket_ = socket(AF_INET, SOCK_STREAM, 0);
		//std::cout << connectSocket_ << std::endl;

		if (ConnectSocket_ < 0) {
			std::cerr << "Error: (CLIENT SIDE)Socket failed with: " << WSAGetLastError();
			WSACleanup();
		}

		sockfd_ = connect(ConnectSocket_, ptr_->ai_addr, (int)ptr_->ai_addrlen);

		if (sockfd_ < 0) {
			std::cerr << "Error: Socket Error" << std::endl;
			closesocket(ConnectSocket_);
			ConnectSocket_ = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(local_);

	if (ConnectSocket_ == INVALID_SOCKET) {
		std::cerr << "Error: Unable to connect to server!" << std::endl;
		WSACleanup();
	}

#endif //  __APPLE__    
}

void TCPNetwork::sendData(std::string message)
{
#ifdef  __APPLE__
    ssize_t n = send(sockfd_, message.c_str(), sizeof(message.c_str()), 0);
    
    if(n < 0)
        std::cerr << "Error: Failed to Send" << std::endl;

#elif _WIN32

	char buffer[1024];
	memset(buffer, 0, 1024);
	memcpy(buffer, message.c_str(), message.length());

	sockfd_ = send(ConnectSocket_, buffer, (int)strlen(buffer), 0);

	if (sockfd_ == SOCKET_ERROR) {
		std::cerr << "Error: Send Failed With Error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket_);
		WSACleanup();
	}

	//shut down the connection so no more data can be sent

	if (sockfd_ == SOCKET_ERROR) {
		std::cerr << "Error: Shutdown failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket_);
	}


#endif //  __APPLE__ 
}

void TCPNetwork::setIP_address(std::string ip)
{
	ip_ = ip;
}

void TCPNetwork::setPortNumber(std::string port)
{
	port_ = port;
}

void TCPNetwork::receiveData()
{

#ifdef  __APPLE__

    long n = recv(sockfd_, recvBuff_, sizeof(recvBuff_), 0);
    
    if(n < 0)
        std::cerr << "Error: Read Error" << std::endl;
    else{
        std::cout << "Recieved: " << n << " BYTES" << std::endl;
        std::cout << "Recieved: " << recvBuff_ << std::endl;
    }

#elif _WIN32 

	sockfd_ = recv(ConnectSocket_, recvBuff_, sizeof(recvBuff_), 0);

	if (sockfd_ > 0) {
		std::cout << "Received: " << sockfd_ << "BYTES" << std::endl;
		std::cout << "Received: " << recvBuff_ << std::endl;
	}
	else if (sockfd_ == 0)
		std::cout << "Warning: Connection Closed" << std::endl;
	else
		std::cerr << "Error: Received failed with error: " << WSAGetLastError() << std::endl;

#endif //  __APPLE__ 
}

