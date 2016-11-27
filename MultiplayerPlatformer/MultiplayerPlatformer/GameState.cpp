//
//  GameState.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 11/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#include "GameState.h"
#include <iostream>
#include <thread>

//Include Game Classes
#include "Player.h"
#include "Bullet.h"
#include "Box.h"
#include "TCPNetwork.hpp"


GameState::GameState()
{
}


GameState::~GameState()
{
}
//Initialize the Game World
bool GameState::createWorld()
{
	try {
		//Create game Window
		window_ = new sf::RenderWindow(sf::VideoMode(1280, 720), "SFML Test");
		//window_->setKeyRepeatEnabled(false);

		//Create / Initialize the Player
		player_ = new Player();
		player_->createSprite();

		//tcpNetwork_.setIP_address(std::string("192.168.56.1"));
		//tcpNetwork_.setPortNumber(std::string("8080"));
		//tcpNetwork_.createSocket();

		return true;
	}
	catch (...) {
		std::cerr << "Error: Failed to create Game World" << std::endl;
		return false;
	}
	return false;
}

void GameState::updateWorld()
{

	//tcpNetwork_.connectToServer();
	//tcpNetwork_.sendData("Hello/n");

	// Create and start the receive thread
	//std::thread tcp_recvThread(&GameState::recvTCPMessage, this);

	sf::RectangleShape ground(sf::Vector2f(1280.f, 20.f));
	ground.setFillColor(sf::Color::Red);
	ground.setPosition(sf::Vector2f(0.f, (float)groundHeight_));


	while (window_->isOpen())
	{
		const sf::Time deltaTime = clock_.restart();
		
		sf::Event event;

		while (window_->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window_->close();
			}				
		}

		if (player_->getPosition().y + 48.5 < groundHeight_ && player_->isJumping() == false)
			player_->fall();

		player_->setMousePosition(sf::Mouse::getPosition(*window_));
		player_->update(event, deltaTime.asSeconds());

		window_->clear(sf::Color(55, 236, 252));
		window_->draw(*player_);
		window_->draw(ground);

		window_->display();
	}

	//When the game window closes, join the threads back to the "main" thread
	//tcp_recvThread.join();

}

//Clean and "free up" Memory
bool GameState::destroyWorld()
{
	try {
		//Delete Game World Variables
		delete window_;

		//Delete Game Physics Variables
 

		//Delete Gameplay Variabels
		delete player_;

		return true;
	}
	catch (...) {

		std::cerr << "Error: Couldn't Destroy Game World" << std::endl;

		return false;
	}
	return false;
}

void GameState::recvTCPMessage()
{
	while (true)
	{
		tcpNetwork_.receiveData();
	}
}
