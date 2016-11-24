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
		window_->setKeyRepeatEnabled(false);
		//Create Physics World and Intialize with Gravity
		b2Vec2 gravity(0.0f, 2.5f);

		world_ = std::make_unique<b2World>(gravity);

		//Make Ground
		groundBodyDef_.position.Set(0.f, 700.f);
		groundBody_ = world_->CreateBody(&groundBodyDef_);

		//Make the ground fixture
		groundBox_.SetAsBox(1280.f, 10.f);
		groundBody_->CreateFixture(&groundBox_, 0.0f);

		//Create / Initialize the Player
		player_ = new Player(world_.get());
		player_->createSprite();

		//tcpNetwork_.setIP_address(std::string("192.168.56.1"));
		//tcpNetwork_.setPortNumber(std::string("8080"));
		//tcpNetwork_.createSocket();

		testBox.init(world_.get(), sf::Vector2f(200, 400), sf::Vector2f(20, 5.f), false);

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
	ground.setPosition(sf::Vector2f(groundBody_->GetPosition().x, groundBody_->GetPosition().y));


	sf::RectangleShape testBoxShape(sf::Vector2f(20.f, 10.f));
	testBoxShape.setPosition(150, 10);
	testBoxShape.setFillColor(sf::Color::Red);

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
	
		world_->Step(1/60.f, 6, 2);

		testBoxShape.setPosition(sf::Vector2f(testBox.getBody()->GetPosition().x, testBox.getBody()->GetPosition().y));

		player_->setMousePosition(sf::Mouse::getPosition(*window_));
		player_->update(event, deltaTime.asSeconds());

		window_->clear(sf::Color(55, 236, 252));
		window_->draw(*player_);
		window_->draw(ground);
		window_->draw(testBoxShape);

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
