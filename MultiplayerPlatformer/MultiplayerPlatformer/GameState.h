//
//  GameState.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 11/11/2016.
//  Copyright � 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

//Include Core Game Libraries
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <queue>
#include <mutex>
#include "TCPNetwork.hpp"
#include "Box.h"

//Include Network classes
#include "TCPNetwork.hpp"
#include "UDPNetwork.h"

//Forward Declaration of Game Classes
class Player;
class Enemy;
class Platform;

class UDPMessenger;

class GameState
{
public:
	GameState();
	~GameState();

	bool createWorld();

	void updateWorld();

	bool destroyWorld();

private:

	void recvTCPMessage();
	void recvUDPMessage();

	void sendUDPMessage();
	void handleUDPMessage();

	void startClient();
	void unReg();
	void setupPlatforms();

	void updatePosMessage();
	std::queue<std::string> udpMsgSendQueue_;
	std::queue<GameDataUDP::DataMessage *> udpMsgRecvQueue_;

	std::string getSendMessage();
	void addMsgToSendQueue(std::string msg);

	GameDataUDP::DataMessage* getRecvMessage();
	void addMsgToRecvQueue(GameDataUDP::DataMessage *dataMsg);

	void spacePressed();

private:

	//Game World Variables
	std::vector<sf::VideoMode> windowDimensions_;
	sf::RenderWindow* window_{ nullptr };
	sf::Clock clock_;
	
	//Network Variables
	TCPNetwork *tcpNetwork_;
	UDPNetwork udpNetwork_;
	UDPMessenger *udpMessenger_{ nullptr };
    
    std::mutex threadMutex_;


	//Game Physics Variables
	b2World *world_{ nullptr };
	b2Vec2 gravity_{ 0.f, 10.f };
	std::vector<Platform *> platforms_{ nullptr };

	const int groundHeight_{ 700 };
	const float gravitySpeed_{ 0.3f };

	//Gameplay Variables
	std::string username_;
	Player *player_{ nullptr };
	std::vector<Enemy *> enemies_{ nullptr };
	int enemyActiveCount_ = 0;
	int enemiesIDXTracker_ = 0;

};

