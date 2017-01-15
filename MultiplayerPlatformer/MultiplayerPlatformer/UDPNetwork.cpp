//
//  UDPNetwork.cpp
//  PlatformerGame
//
//  Created by MUNRO, CHRISTOPHER on 02/12/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "UDPNetwork.h"
#include "GameDataUDP.pb.h"
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

	//Setup address structure
	ser_addr_.sin_family = AF_INET;
	ser_addr_.sin_port = htons(port_);

	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	const int trueValue = 1;
	setsockopt(socket_, SOL_SOCKET, SO_REUSEPORT, &trueValue, sizeof(trueValue));

	if (socket_ < 0)
		std::cerr << "Error: Failed to Create Socket" << std::endl;

	//if(bind(socket_, (struct sockaddr *)&ser_addr_, sizeof(ser_addr_) ) < 0)
	//    std::cerr << "Error: Failed to Bind the Socket" << std::endl;

#elif _WIN32

	ZeroMemory(&recvAddr_, sizeof(recvAddr_));

	//Setup address structure
	recvAddr_.sin_family = AF_INET;
	recvAddr_.sin_port = htons(8082);
	//recvAddr_.sin_addr.S_un.S_addr = inet_addr(ip_.c_str());
	recvAddr_.sin_addr.S_un.S_addr = INADDR_ANY;

	//Create a SOCKET for connecting to server
	socket_ = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

	if (socket_ == INVALID_SOCKET) {
		std::cerr << "Error: Invalid Socket: " << WSAGetLastError() << std::endl;
		WSACleanup();
	}
	char broadcast = 'a';
	char reuse = 'b';
	setsockopt(socket_, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
	setsockopt(socket_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	int result = bind(socket_, (SOCKADDR *)&recvAddr_, sizeof(recvAddr_));

	//std::cout << "UDP RESULT: " << result << std::endl;

	if (result == SOCKET_ERROR) {
		std::cerr << "Error: UDP Socket Bind Failed with Error: " << WSAGetLastError() << std::endl;
	}

#endif //  __APPLE__
}

GameDataUDP::DataMessage* UDPNetwork::receiveData()
{
#ifdef __APPLE__

	std::cout << "Listening for UDP data...." << std::endl;

	int s = recv(socket_, recvBuff_, sizeof(recvBuff_), 0);
	//int s = recvfrom(socket_, recvBuff_, sizeof(recvBuff_), 0, (struct sockaddr *) &ser_addr_, (unsigned int *)(&ser_addr_));

	if (s < 0) {
		std::cerr << "Error: UDP Read Error" << s << std::endl;
	}
	else {
		std::cout << "Received UDP: " << socket_ << " BYTES" << std::endl;
		std::cout << "Received UDP: " << recvBuff_ << std::endl;
	}

#elif _WIN32
	if (socket_ > 0) {
		GameDataUDP::DataMessage* dataMsg = new GameDataUDP::DataMessage();

		sockaddr_in *from;
		int size = sizeof(recvAddr_);
		int result = recvfrom(socket_, recvBuff_, sizeof(recvBuff_), 0, (sockaddr *)&recvAddr_, &size);

		//int result = recv(socket_, recvBuff_, sizeof(recvBuff_), 0);

		if (result > 0) {
			char *newBuffer = new char[result];

			memcpy(newBuffer, recvBuff_, result);

			if (!dataMsg->ParseFromArray(newBuffer, result)) {
				std::cout << "Error: Parse Failed" << std::endl;
				return nullptr;
			}
		}

		if (result == SOCKET_ERROR)
			std::cerr << "Error: UDP Recvfrom failed with error: " << WSAGetLastError() << std::endl;
		else {
			//std::cout << "Received UDP: " << socket_ << " BYTES" << std::endl;
			//std::cout << "Received UDP: " << newBuffer << std::endl;
			return dataMsg;
		}
	}
	else
		std::cerr << "Error: UDP Connection Lost" << std::endl;

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

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr(ip_.c_str());
	addr.sin_port = htons(port_);

	auto a = message.size();

	if (sendto(socket_, message.data(), message.size(), 0, (struct sockaddr *) &addr, sizeof(addr)) == SOCKET_ERROR) {
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

bool UDPNetwork::isConnected() const
{
#ifdef __APPLE__

	if (sockfd_ > 0) {
		return true;
	}
	return false;

#elif _WIN32

	if (socket_ > 0) {
		return true;
	}
	return false;

#endif
}
