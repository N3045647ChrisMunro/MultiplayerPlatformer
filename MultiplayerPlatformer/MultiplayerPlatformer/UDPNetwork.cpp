//
//  UDPNetwork.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 01/12/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "UDPNetwork.h"
#include <string>


UDPNetwork::UDPNetwork()
{
	memset(recvBuff_, '0', sizeof(recvBuff_));

#ifdef __APPLE__

	memset(&serv_addr_, '0', sizeof(serv_addr_));

#elif _WIN32

	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Error: WSAStartup Failed: " << WSAGetLastError() << std::endl;
		return;
	}

#endif

}


UDPNetwork::~UDPNetwork()
{
#ifdef  __APPLE__


#elif _WIN32

	closesocket(socket_);
	WSACleanup();

#endif //  __APPLE__
}

void UDPNetwork::createSocket()
{
#ifdef  __APPLE__

	sockfd_ = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd_ < 0)
		std::cerr << "Error: Failed to Create Socket" << std::endl;

#elif _WIN32

	ZeroMemory(&si_other_, sizeof(si_other_));

	//Setup address structure
	si_other_.sin_family = AF_INET;
	si_other_.sin_port = htons(port_);
	si_other_.sin_addr.S_un.S_addr = inet_addr(ip_.c_str());

	//Create a SOCKET for connecting to server
	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_ == INVALID_SOCKET) {
		std::cerr << "Error: Invalid Socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
	}

#endif //  __APPLE__
}

void UDPNetwork::receiveData()
{
	std::cout << "Listeng for udp data..." << std::endl;
	int s = recvfrom(socket_, recvBuff_, sizeof(recvBuff_), 0, (struct sockaddr *) &si_other_, &slen_);
	if (s > 0) {
		std::cout << "Received: " << socket_ << "BYTES" << std::endl;
		std::cout << "Received: " << recvBuff_ << std::endl;
	}
	else if(s == 0)
		std::cerr << "Warning: UDP Connection Closed" << std::endl;
	else
		std::cerr << "Error: UDPReceive Failed: " << WSAGetLastError() << std::endl;

}

void UDPNetwork::sendData(std::string message)
{
	char buffer[1024];
	memset(buffer, 0, 1024);
	memcpy(buffer, message.c_str(), message.length());

	if (sendto(socket_, buffer, strlen(buffer), 0, (struct sockaddr *) &si_other_, slen_) == SOCKET_ERROR) {
		std::cerr << "Error: UDPSend Failed: " << WSAGetLastError() << std::endl;
	}
}

void UDPNetwork::setIP_address(std::string ip)
{
	ip_ = ip;
}

void UDPNetwork::setPortNumber(int port)
{
	port_ = port;
}
