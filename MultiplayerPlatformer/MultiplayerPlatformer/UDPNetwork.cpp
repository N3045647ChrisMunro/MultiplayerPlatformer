//
//  UDPNetwork.cpp
//  PlatformerGame
//
//  Created by MUNRO, CHRISTOPHER on 02/12/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "UDPNetwork.h"
#include <string>


UDPNetwork::UDPNetwork()
{
	memset(recvBuff_, '0', sizeof(recvBuff_));

#ifdef __APPLE__

	//memset(&serv_addr_, '0', sizeof(serv_addr_));

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

	//ZeroMemory(&si_other_, sizeof(si_other_));

	//Setup address structure
	ser_addr_.sin_family = AF_INET;
	ser_addr_.sin_port = htons(port_);

	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_ < 0)
		std::cerr << "Error: Failed to Create Socket" << std::endl;

#elif _WIN32

	ZeroMemory(&local_, sizeof(local_));

	//Setup address structure
	local_.sin_family = AF_INET;
	local_.sin_port = htons(port_);
	local_.sin_addr.s_addr = inet_addr(ip_.c_str());

	dest_.sin_family = AF_INET;
	dest_.sin_addr.s_addr = inet_addr(ip_.c_str());
	dest_.sin_port = htons(port_);

	//Create a SOCKET for connecting to server
	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_ == INVALID_SOCKET) {
		std::cerr << "Error: Invalid Socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
	}

	//Bind the socket to the local address
	bind(socket_, (sockaddr *)&local_, sizeof(local_));

#endif //  __APPLE__
}

void UDPNetwork::receiveData()
{
#ifdef __APPLE__

	int s = recv(socket_, recvBuff_, sizeof(recvBuff_), 0);
	//recvfrom(socket_, recvBuff_, sizeof(recvBuff_), 0, (struct sockaddr *) &si_other_, &slen_);

	if (s > 0) {
		std::cout << "Received: " << socket_ << "BYTES" << std::endl;
		std::cout << "Received: " << recvBuff_ << std::endl;
	}
	else if (s == 0)
		std::cerr << "Warning: UDP Connection Closed" << std::endl;
	else
		std::cerr << "Error: UDPReceive Failed: " << std::endl;


#elif _WIN32

	std::cout << "Listeng for udp data..." << std::endl;
	int s = recvfrom(socket_, recvBuff_, sizeof(recvBuff_), 0, (struct sockaddr *) &dest_, (int*)sizeof(dest_));
	if (s > 0) {
		std::cout << "Received: " << socket_ << "BYTES" << std::endl;
		std::cout << "Received: " << recvBuff_ << std::endl;
	}
	else if (s == 0)
		std::cerr << "Warning: UDP Connection Closed" << std::endl;
	else
		std::cerr << "Error: UDPReceive Failed: " << WSAGetLastError() << std::endl;


#endif // __APPLE__

}

void UDPNetwork::sendData(std::string message)
{
	char buffer[1024];
	memset(buffer, 0, 1024);
	memcpy(buffer, message.c_str(), message.length());

#ifdef __APPLE__

	int s = sendto(socket_, buffer, sizeof(buffer), 0, (struct sockaddr *)&ser_addr_, sizeof(ser_addr_));

	std::cout << "UDP SOCKET " << s << std::endl;

	if (s > 0)
		std::cout << "Sent UDP " << std::endl;
	else
		std::cerr << "Error: UDP Send failed: " << s << std::endl;


#elif _WIN32

	if (sendto(socket_, buffer, strlen(buffer), 0, (struct sockaddr *) &dest_, sizeof(dest_)) == SOCKET_ERROR) {
		std::cerr << "Error: UDPSend Failed: " << WSAGetLastError() << std::endl;
	}

#endif // __APPLE__

}

void UDPNetwork::setIP_address(std::string ip)
{
	ip_ = ip;
}

void UDPNetwork::setPortNumber(int port)
{
	port_ = port;
}