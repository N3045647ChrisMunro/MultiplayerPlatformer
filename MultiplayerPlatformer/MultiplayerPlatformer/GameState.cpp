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
#include <fstream>

//Protobuf
#include "GameDataTCP.pb.h"
#include "GameDataUDP.pb.h"

//Include Game Classes
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Box.h"
#include "Platform.h"

#include "google/protobuf/compiler/parser.h"

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

		//Creaate box2D physics world
		world_ = new b2World(gravity_);
		world_->SetAllowSleeping(true);

		//Create / Initialize the Player
		player_ = new Player();
		player_->createSprite(world_);
		player_->setUsername(username_);

		const int MAX_ENEMIES = 10;
		enemies_.clear();
		enemies_.resize(MAX_ENEMIES);

		for (unsigned int i = 0; i < enemies_.size(); i++) {
			enemies_[i] = new Enemy();
			enemies_[i]->createSprite(world_);
		}

		udpNetwork_.setIP_address("192.168.170.1");
		udpNetwork_.setPortNumber(8081);
		udpNetwork_.createSocket();

		startClient();

		//Create game Window
		windowDimensions_ = sf::VideoMode::getFullscreenModes();
		window_ = new sf::RenderWindow(windowDimensions_[0], "SFML Test");


		//Create all the platfroms for the game world
		setupPlatforms();

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

	sf::Vector2f tempPos = player_->getPosition();

	while (window_->isOpen())
	{
		window_->clear(sf::Color(55, 236, 252));

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

		if (player_->getPosition() != tempPos) {
			updatePosMessage();

			tempPos = player_->getPosition();
		}

		player_->setMousePosition(sf::Mouse::getPosition(*window_));
		player_->update(event, deltaTime.asSeconds());

		for (auto p : platforms_) {
			window_->draw(*p);
		}

		if (enemies_.size() > 0) {
			for (auto e : enemies_) {
				if (e->isAlive()) {
					window_->draw(*e);
					e->update(deltaTime.asSeconds());
				}
			}
		}

		window_->draw(*player_);

		window_->display();
	}

	unReg();

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

		for (unsigned int i = 0; i < enemies_.size(); i++) {
			delete enemies_[i];
		}

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
		if (tcpNetwork_->isConnected()) {

			GameDataTCP::DataMessage* dataMsg = new GameDataTCP::DataMessage();
			GameDataTCP::NewPlayerReg* reg = new GameDataTCP::NewPlayerReg();
			dataMsg->Clear();

			dataMsg = tcpNetwork_->receiveData();

			//Handle the Message
			try {
				//Check to see if the message is aand update about newly registered players
				if (dataMsg->newplayerreg_size() > 0) {

					for (unsigned int i = 0; i < dataMsg->newplayerreg_size(); i++) {

						std::string status = dataMsg->newplayerreg().Get(i).status();

						if (status == "NewReg") {
							std::cout << "New Player Registered" << std::endl;

							enemies_[enemiesIDXTracker_]->setAliveStatus(true);
							enemies_[enemiesIDXTracker_]->setUserName(dataMsg->newplayerreg().Get(0).username());
							enemiesIDXTracker_++;

							dataMsg->Clear();
						}
					}

				}
			}
			catch (...) {
				std::cerr << "Error: Exception in TCP Recv Thread" << std::endl;
			}

		}
		else
			std::cerr << "Error: Connection Lost" << std::endl;
	}
}

void GameState::recvUDPMessage()
{
	while (true)
	{
		if (udpNetwork_.isConnected()) {
			GameDataUDP::DataMessage* dataMsg = new GameDataUDP::DataMessage();

			dataMsg = udpNetwork_.receiveData();

			if (dataMsg->has_positionupdate()) {
				std::cout << dataMsg->positionupdate().xpos() << ", " << dataMsg->positionupdate().ypos() << std::endl;
			}
			else {
				std::cout << "UDP Proto: pos update is null" << std::endl;
			}
		}
		else
			std::cerr << "Error: Connection Lost (UDP)" << std::endl;

	}
}

void GameState::startClient()
{
	//Set up the network connection to the server
	tcpNetwork_ = new TCPNetwork();
	tcpNetwork_->setIP_address(std::string("192.168.170.1"));
	tcpNetwork_->setPortNumber(std::string("8080"));
	tcpNetwork_->createSocket();
	tcpNetwork_->connectToServer();

	GameDataTCP::DataMessage* dataMsg = new GameDataTCP::DataMessage();
	GameDataTCP::Register* regData = new GameDataTCP::Register();
	std::string name, pass;
	std::string status;

	std::cout << "Hello, and Welcome To Chris Munro's super awesomely incredible unrivalled Multiplayer Platform Shooter" << std::endl;
	std::cout << "1: Register" << std::endl;
	std::cout << "2: Login" << std::endl;
	std::cout << ">>: ";

	char input;
	std::cin >> input;
	std::cout << std::endl;

	std::string buff_string;
	std::string recvString;

	switch (input) {
	case '1':
		std::cout << "reg" << std::endl;

		std::cout << "Username: ";
		std::cin >> name;

		std::cout << "Password: ";
		std::cin >> pass;

		std::cout << std::endl;
		username_ = name;

		regData->set_username(name);
		regData->set_password(pass);

		dataMsg->set_allocated_register_(regData);

		buff_string = dataMsg->SerializeAsString();

		std::cout << buff_string << std::endl;

		//Send a registration request to the server
		tcpNetwork_->sendData(buff_string);
		dataMsg = tcpNetwork_->receiveData();

		//Handle the message
		if (dataMsg->newplayerreg_size() > 0) {

			//Check to see wether the registration was successful
			status = dataMsg->newplayerreg().Get(0).status();

			if (status == "Success") {
				std::cout << "Registration Successful" << std::endl;

				if (dataMsg->newplayerreg_size() > 1) {
					for (unsigned int i = 1; i < dataMsg->newplayerreg_size(); i++) {
						status = dataMsg->newplayerreg().Get(i).status();
						if (status == "oldClients") {
							enemies_[i - 1]->setAliveStatus(true); // - 1 so we start at the first index
							enemies_[i - 1]->setUserName(dataMsg->newplayerreg().Get(i).username());

							std::cout << enemies_[i - 1]->getUserName() << std::endl;
						}

					}

				}
			}
			else if (status == "Failed") {
				std::cout << "Sorry That Username is Taken" << std::endl;
				startClient();
			}
			//else if (status == "oldClients")
			//{
			//	enemies_[i - 1]->setAliveStatus(true); // - 1 so we start at the first index
			//	enemies_[i - 1]->setUserName(dataMsg->newplayerreg().Get(i).username());
			//	
			//	dataMsg->Clear();
			//}

		}
		break;
	case '2':
		std::cout << "login" << std::endl;
		break;
	default:
		std::cout << "Sorry, I dont understand that" << std::endl;
		break;
	}

	//delete regData;
	//delete dataMsg;
}

void GameState::unReg()
{
	GameDataTCP::DataMessage* dataMsg = new GameDataTCP::DataMessage();
	GameDataTCP::NewPlayerReg* unRegMsg = new GameDataTCP::NewPlayerReg();

	unRegMsg->set_username(player_->getUsername());
	unRegMsg->set_status("UnReg");

	dataMsg->mutable_newplayerreg()->AddAllocated(unRegMsg);

	std::string buffer = dataMsg->SerializeAsString();

	tcpNetwork_->sendData(buffer);
}

void GameState::setupPlatforms()
{
	platforms_.clear();

	//Ground
	Platform* groundPlatform = new Platform();
	groundPlatform->createGroundPlane(world_, b2Vec2((window_->getSize().x / 2) / PPM, (window_->getSize().y - 14.f) / PPM), window_->getSize().x / PPM, 34.f / PPM);

	platforms_.push_back(groundPlatform);

	//Platforms at the bottom
	const int numOfBotPlats = 3;
	Platform* botPlatforms = new Platform[numOfBotPlats];

	float botPlayWidth = 32 * 9;
	int xPos = (window_->getSize().x / 3) - botPlayWidth;
	for (unsigned int i = 0; i < numOfBotPlats; i++) {

		botPlatforms[i].createPlatform(world_, b2Vec2(xPos / PPM, (groundPlatform->getPosition().y - 72.f) / PPM), botPlayWidth / PPM, 32.f / PPM);

		platforms_.push_back(&botPlatforms[i]);
		xPos += botPlayWidth * 2;
	}

	//Center platform
	Platform* centerPlatform = new Platform();
	const float centerX = window_->getSize().x / 2;
	const float centerY = window_->getSize().y / 2;
	const float centerWidth = 32 * 6;
	centerPlatform->createPlatform(world_, b2Vec2(centerX / PPM, centerY / PPM), centerWidth / PPM, 32.f / PPM);

	platforms_.push_back(centerPlatform);

	Platform* leftTop = new Platform();
	const float leftTopWidth = 480.f;
	leftTop->createPlatform(world_, b2Vec2((leftTopWidth - 32 * 3) / PPM, 224.f / PPM), leftTopWidth / PPM, 32.f / PPM);

	platforms_.push_back(leftTop);

	Platform* rightTop = new Platform();
	const float rightTopWidth = 480.f;
	const float rightTopPos = window_->getSize().x - (rightTopWidth / 2) - 32 * 3;
	rightTop->createPlatform(world_, b2Vec2(rightTopPos / PPM, 224.f / PPM), rightTopWidth / PPM, 32.f / PPM);

	platforms_.push_back(rightTop);

}

void GameState::updatePosMessage()
{
	GameDataUDP::DataMessage* dataMsg = new GameDataUDP::DataMessage();
	GameDataUDP::PositionUpdate* posData = new GameDataUDP::PositionUpdate();

	posData->set_xpos(player_->getPosition().x);
	posData->set_ypos(player_->getPosition().y);
	dataMsg->set_allocated_positionupdate(posData);

	std::string stringBuff = dataMsg->SerializeAsString();

	udpNetwork_.sendData(stringBuff);



}

