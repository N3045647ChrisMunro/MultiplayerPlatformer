//
//  TCPNetwork.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 10/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#include "TCPNetwork.hpp"
#include <fstream>
#include <string>

TCPNetwork::TCPNetwork()
{

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
	sockfd_ = send(ConnectSocket_, message.data(), strlen(message.c_str()), 0);

	if (sockfd_ == SOCKET_ERROR) {
		std::cerr << "Error: Send Failed With Error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket_);
		WSACleanup();
	}
#endif //  __APPLE__ 
}

void TCPNetwork::sendFile(std::string filename)
{
#ifdef  __APPLE__
	ssize_t n = send(sockfd_, message.c_str(), sizeof(message.c_str()), 0);

	if (n < 0)
		std::cerr << "Error: Failed to Send" << std::endl;

#elif _WIN32

	//Get the file size
	std::fstream fileToSend(filename, std::ios::in | std::ios::binary);
	if (!fileToSend)
		std::cerr << "Error: TCP Failed to open file: " << filename << std::endl;
	fileToSend.seekg(0, std::ios::end);
	unsigned int fileSize = fileToSend.tellg();
	fileToSend.close();

	//Get the File
	char *buffer = new char[fileSize];
	//memset(buffer, 0, sizeof(buffer));
	fileToSend.open(filename, std::ios::binary);
	fileToSend.seekg(0, std::ios::beg);
	fileToSend.read(buffer, fileSize);
	fileToSend.close();

	//Send file in Chunks
	const int FILE_CHUNK_SIZE = 2000;
	unsigned int bytesSent = 0;
	int bytesToSend = 0;

	while (bytesSent < fileSize) {
	
		if (fileSize - bytesSent >= FILE_CHUNK_SIZE)
			bytesToSend = FILE_CHUNK_SIZE;
		else
			bytesToSend = fileSize - bytesSent;

		send(ConnectSocket_, buffer + bytesSent, bytesToSend, 0);
		bytesSent += bytesToSend;

	}
	
	std::cout << "Sent File: " << buffer << std::endl;
	
	delete[] buffer;
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

bool TCPNetwork::isConnected() const
{
#ifdef __APPLE__

	if (sockfd_ > 0) {
		return true;
	}
	return false;

#elif _WIN32

	if (ConnectSocket_ > 0) {
		return true;
	}
	return false;

#endif
}

GameDataTCP::DataMessage* TCPNetwork::receiveData()
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

	if (ConnectSocket_ > 0) {
		GameDataTCP::DataMessage* dataMsg = new GameDataTCP::DataMessage();

		sockfd_ = recv(ConnectSocket_, recvBuff_, sizeof(recvBuff_), 0);

		if (sockfd_ > 0) {

			char *newBuffer = new char[sockfd_];

			memcpy(newBuffer, recvBuff_, sockfd_);

			if (!dataMsg->ParseFromArray(newBuffer, sockfd_)) {
				std::cerr << "Error: Parse Failed (TCP)" << std::endl;
				return nullptr;
			}

			return dataMsg;

		}
		else if (sockfd_ == 0)
			std::cout << "Warning: Connection Closed" << std::endl;
		else
			std::cerr << "Error: Received failed with error: " << WSAGetLastError() << std::endl;
	}
	else {
		std::cerr << "Error: TCP Connection Lost " << std::endl;
	}
#endif //  __APPLE__ 
}

