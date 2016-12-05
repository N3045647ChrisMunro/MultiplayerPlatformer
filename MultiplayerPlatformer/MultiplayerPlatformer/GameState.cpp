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
#include "Platform.h"


#define PPM 30.f //Pixels per meter

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

		//Creaate box2D physics world
		world_ = new b2World(gravity_);
		world_->SetAllowSleeping(true);

		//Create / Initialize the Player
		player_ = new Player();
		player_->createSprite(world_);

		tcpNetwork_ = new TCPNetwork();
		tcpNetwork_->setIP_address(std::string("192.168.170.1"));
		tcpNetwork_->setPortNumber(std::string("8080"));
		tcpNetwork_->createSocket();

		tcpNetwork_->connectToServer();
		tcpNetwork_->sendData("Hello");

		//udpNetwork_.setIP_address(std::string("192.168.56.1"));
		udpNetwork_.setPortNumber(8081);
		udpNetwork_.createSocket();
		udpNetwork_.sendData("Hello UDP");

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
	//std::thread udp_recvThread(&GameState::recvUDPMessage, this);

	//TODO: Remove this 
	Platform groundPlatform;
	groundPlatform.createSmall(world_, b2Vec2(700.f / PPM, 400.f / PPM));

	Platform platform;
	platform.createSmall(world_, b2Vec2(500.f / PPM, 400.f / PPM));

	//box
	Box Box2;
	Box2.init(world_, { 500.f / PPM, 100.f / PPM }, { 25.f / PPM, 25.f / PPM }, true);

	sf::RectangleShape shape(sf::Vector2f(25.f, 25.f));
	shape.setOrigin(12.5f, 12.5f);
	shape.setFillColor(sf::Color::Green);

	//Ground
	sf::RectangleShape groundSF(sf::Vector2f(1280.f, 25.f));
	groundSF.setOrigin(640.f, 12.5f);
	groundSF.setFillColor(sf::Color::Red);

	Box groundBox;
	groundBox.init(world_, { 640.f / PPM, 600.f / PPM }, { 1280.f / PPM, 25.f / PPM }, false);

	groundSF.setPosition(sf::Vector2f(groundBox.getBody()->GetPosition().x * PPM, groundBox.getBody()->GetPosition().y * PPM));

	//platforms_.push_back(&groundPlatform);
	//platforms_.push_back(&platform);

	while (window_->isOpen())
	{
		sf::Time deltaTime = clock_.restart();
		
		world_->Step(deltaTime.asSeconds(), 8, 3);

		sf::Event event;
		while (window_->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window_->close();
			}				
		}
	
		shape.setPosition(Box2.getBody()->GetPosition().x * PPM, Box2.getBody()->GetPosition().y * PPM);

		player_->setMousePosition(sf::Mouse::getPosition(*window_));
		player_->update(event, deltaTime.asSeconds());

		window_->clear(sf::Color(55, 236, 252));
		window_->draw(groundSF);
		window_->draw(shape);
		window_->draw(groundPlatform);
		window_->draw(platform);
		
		window_->draw(*player_);

		window_->display();
	}

	//When the game window closes, join the threads back to the "main" thread
	tcp_recvThread.join();
	//udp_recvThread.join();
}

//Clean and "free up" Memory
bool GameState::destroyWorld()
{
	try {
		//Delete Game World Variables
		delete window_;

		//Delete Game Physics Variables
		delete world_;

		//Delete Gameplay Variabels
		delete player_;

		delete tcpNetwork_;

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
		tcpNetwork_->receiveData();
	}
}

void GameState::recvUDPMessage()
{
	while (true)
	{
		udpNetwork_.receiveData();
	}
}

