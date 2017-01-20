//
//  UDPMessenger.h
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 20/01/2017.
//  Copyright � 2017 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

#include <queue>
#include <mutex>

#include "GameDataUDP.pb.h"

class UDPMessenger
{
public:
	UDPMessenger();
	~UDPMessenger();

	void addMsgToSendQueue(std::string msg);
	std::string getSendMessage();
	
	void addMsgToRecvQueue(GameDataUDP::DataMessage *dataMsg);
	GameDataUDP::DataMessage *getRecvMessage();


private:
	
	std::mutex threadMutex_;
	
	std::queue<std::string> udpMsgSendQueue_;
	std::queue<GameDataUDP::DataMessage *> udpMsgRecvQueue_;
	

};

