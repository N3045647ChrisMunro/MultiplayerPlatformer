//
//  UDPMessenger.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 20/01/2017.
//  Copyright � 2017 MUNRO, CHRISTOPHER. All rights reserved.
//

#include "UDPMessenger.h"


UDPMessenger::UDPMessenger()
{
}


UDPMessenger::~UDPMessenger()
{
}

void UDPMessenger::addMsgToSendQueue(std::string msg)
{
	threadMutex_.lock();

	udpMsgSendQueue_.push(msg);

	threadMutex_.unlock();
}

std::string UDPMessenger::getSendMessage()
{
	if (udpMsgSendQueue_.size() > 0) {
		
		threadMutex_.lock();

		std::string message;
		message = udpMsgSendQueue_.front();
		udpMsgSendQueue_.pop();

		threadMutex_.unlock();

		return message;
	}
	return "";
}

void UDPMessenger::addMsgToRecvQueue(GameDataUDP::DataMessage * dataMsg)
{
	threadMutex_.lock();

	udpMsgRecvQueue_.push(dataMsg);

	threadMutex_.unlock();
}

GameDataUDP::DataMessage * UDPMessenger::getRecvMessage()
{
	if (udpMsgRecvQueue_.size() > 0) {

		threadMutex_.lock();

		GameDataUDP::DataMessage *message = udpMsgRecvQueue_.front();
		udpMsgRecvQueue_.pop();

		threadMutex_.unlock();

		return message;
	}
	return nullptr;
}
