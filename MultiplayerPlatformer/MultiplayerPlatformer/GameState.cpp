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

		//Create Physics World and Intialize with Gravity
		b2Vec2 gravity(0.0f, -9.81);

		world_ = std::make_unique<b2World>(gravity);

		//Make Ground
		groundBodyDef_.position.Set(0.0f, -10.f);
		groundBody_ = world_->CreateBody(&groundBodyDef_);

		//Make the ground fixture
		groundBox_.SetAsBox(1280.0f, 500.0f);
		groundBody_->CreateFixture(&groundBox_, 0.0f);

		//Create / Initialize the Player
		player_ = new Player(world_.get());
		player_->createSprite();

		Bullet bullet;

		Box newBox;
		newBox.init(world_.get(), sf::Vector2f(350.f, 100.f), sf::Vector2f(72.f, 97.f));

		sf::RectangleShape myBox(newBox.getDimensions());
		myBox.setFillColor(sf::Color::Red);

		tcpNetwork_.setIP_address(std::string("192.168.56.1"));
		tcpNetwork_.setPortNumber(std::string("8080"));
		tcpNetwork_.createSocket();
		tcpNetwork_.connectToServer();
		tcpNetwork_.sendData("Hello/n");

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
	// Create and start the receive thread
	std::thread tcp_recvThread(&GameState::recvTCPMessage, this);

	while (window_->isOpen())
	{
		sf::Event event;
		while (window_->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window_->close();
			}
		}
		
		const sf::Time deltaTime = clock_.restart();

		player_->setMousePosition(sf::Mouse::getPosition(*window_));
		player_->update(event, deltaTime.asSeconds());

		world_->Step(deltaTime.asSeconds(), 6, 2);

		window_->clear(sf::Color(55, 236, 252));
		window_->draw(*player_);

		window_->display();
	}

	//When the game window closes, join the threads back to the "main" thread
	tcp_recvThread.join();

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
