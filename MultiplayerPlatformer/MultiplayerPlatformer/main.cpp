//
//  main.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 04/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//


#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include "Player.h"
#include "Bullet.h"
#include <Box2D\Box2D.h>
#include "Box.h"
#include "TCPNetwork.hpp"
#include "GameState.h"

int main()
{
	GameState *gameState = new GameState();

	if (gameState->createWorld())
	{
		gameState->updateWorld();
	}
	else {
		std::cerr << "Error: Something went wrong" << std::endl;
	}
	return 0;
}