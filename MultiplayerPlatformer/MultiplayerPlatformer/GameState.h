//
//  GameState.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 11/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

//Include Core Game Libraries
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include "TCPNetwork.hpp"
#include "Box.h"

//Include Network classes
#include "TCPNetwork.hpp"
#include "UDPNetwork.h"


//Forward Declaration of Game Classes
class Player;
class Platform;

class GameState
{
public:
	GameState();
	~GameState();

	bool createWorld();

	void updateWorld();

	bool destroyWorld();

private:

	enum GameEntities { player, bullet, platform, enemy};
	void recvTCPMessage();
	void recvUDPMessage();

private:

	//Game World Variables
	sf::RenderWindow* window_{ nullptr };
	sf::Clock clock_;
	
	//Network Variables
	TCPNetwork tcpNetwork_;
	UDPNetwork udpNetwork_;

	//Game Physics Variables
	b2World *world_{ nullptr };
	b2Vec2 gravity_{ 0.f, 10.f };
	std::vector<Platform *> platforms_{ nullptr };

	const int groundHeight_{ 700 };
	const float gravitySpeed_{ 0.3f };


	//Gameplay Variables
	Player *player_{ nullptr };

};

