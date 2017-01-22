//
//  UDPMessenger.cpp
//  PlatformerGame
//
//  Created by MUNRO, CHRISTOPHER on 20/01/2017.
//  Copyright © 2017 MUNRO, CHRISTOPHER. All rights reserved.
//

#include "UDPMessenger.hpp"

UDPMessenger::UDPMessenger()
{
}

UDPMessenger::~UDPMessenger()
{
}

//Add a message to the receive queue
void UDPMessenger::addMsgToRecvQueue(GameDataUDP::DataMessage *dataMsg)
{
    threadMutex_.lock();
    
    udpMsgRecvQueue_.push(dataMsg);
    
    threadMutex_.unlock();
}

//Get a message from the receive queue
GameDataUDP::DataMessage* UDPMessenger::getRecvMessage()
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

//Add a message to the send queue
void UDPMessenger::addMsgToSendQueue(std::string msg)
{
    threadMutex_.lock();
    
    udpMsgSendQueue_.push(msg);
   
    
    threadMutex_.unlock();
}

//Get a message form the send queue
std::string UDPMessenger::getSendMessage()
{
    if (udpMsgSendQueue_.size() > 0) {
        threadMutex_.lock();
        
        std::string message;
        message = udpMsgSendQueue_.front();
        udpMsgSendQueue_.pop();
        
        //std::cout << "got from send " << std::endl;
        
        threadMutex_.unlock();
        
        return message;
    }
    return "";
}
