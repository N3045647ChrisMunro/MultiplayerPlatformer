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
#include <Box2D\Box2D.h>
#include "TCPNetwork.hpp"
#include "Box.h"


//Forward Declaration of Game Classes
class Player;

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

private:

	//Game World Variables
	sf::RenderWindow* window_{ nullptr };
	TCPNetwork tcpNetwork_;
	sf::Clock clock_;


	//Game Physics Variables
	std::unique_ptr<b2World> world_;
	b2BodyDef groundBodyDef_;
	b2Body* groundBody_{ nullptr };
	b2PolygonShape groundBox_;
	Box testBox;


	//Gameplay Variables
	Player *player_{ nullptr };

};

