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

		startClient();

		//Create game Window
		windowDimensions_ = sf::VideoMode::getFullscreenModes();
		window_ = new sf::RenderWindow(windowDimensions_[0], "SFML Test");

		//Creaate box2D physics world
		world_ = new b2World(gravity_);
		world_->SetAllowSleeping(true);

		//Create all the platfroms for the game world
		setupPlatforms();

		//Create / Initialize the Player
		player_ = new Player();
		player_->createSprite(world_);

		udpNetwork_.setIP_address("192.168.170.1");
		udpNetwork_.setPortNumber(8081);
		udpNetwork_.createSocket();

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
	//std::thread tcp_recvThread(&GameState::recvTCPMessage, this);
	//std::thread udp_recvThread(&GameState::recvUDPMessage, this);

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
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Tab))
			{
				udpNetwork_.sendData("Hello from udp client: ");
			}
		}

		player_->setMousePosition(sf::Mouse::getPosition(*window_));
		player_->update(event, deltaTime.asSeconds());

		window_->clear(sf::Color(55, 236, 252));
		window_->draw(groundSF_);

		for (auto p : platforms_) {
			window_->draw(*p);
		}
		
		window_->draw(*player_);
		window_->display();
	}

	//When the game window closes, join the threads back to the "main" thread
	//tcp_recvThread.join();
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

void GameState::startClient()
{
	//Set up the network connection to the server
	tcpNetwork_ = new TCPNetwork();
	tcpNetwork_->setIP_address(std::string("192.168.170.1"));
	tcpNetwork_->setPortNumber(std::string("8080"));
	tcpNetwork_->createSocket();
	tcpNetwork_->connectToServer();

	test::Reg reg;
	std::string name, pass;

	//Open the proto file
	std::fstream fileInput("test.proto", std::ios::in | std::ios::binary);
	if (!fileInput) {
		std::cerr << "ERROR: Failed to load proto file" << std::endl;
	}
	else if (!reg.ParseFromIstream(&fileInput)) {
		std::cerr << "ERROR: Failed to parse proto file" << std::endl;
	}

	std::fstream fileOutput("test1.proto", std::ios::out | std::ios::trunc | std::ios::binary);

	std::cout << "Hello, and Welcome To Chris Munro's super awesomely incredible unrivalled Multiplayer Platform Shooter" << std::endl;
	std::cout << "1: Register" << std::endl;
	std::cout << "2: Login" << std::endl;
	std::cout << ">>: ";

	char input;
	std::cin >> input;
	std::cout << std::endl;

	std::string buff_string;
	std::string messageString;
	std::string recvString;
	int size;

	switch (input) {
		case '1':
			std::cout << "reg" << std::endl;

			std::cout << "Username: ";
			std::cin >> name;

			std::cout << "Password: ";
			std::cin >> pass;

			std::cout << std::endl;

			username_ = name;
			password_ = pass;

			reg.set_username(name);
			reg.set_password(pass);

			messageString = "reg:" + name + ":" + pass;

			if (!reg.SerializeToOstream(&fileOutput)) {
				std::cerr << "ERROR: Failed to write proto file" << std::endl;
			}

			fileOutput.close();

			buff_string = reg.SerializeAsString();
			size = buff_string.size();

			std::cout << buff_string << std::endl;
			//Send a registration request to the server
			//tcpNetwork_->sendFile("test1.proto");
			tcpNetwork_->sendData(buff_string, size);
			recvString = tcpNetwork_->receiveData();

			//Check to see wether the registration was successful
			if (recvString == "reg:OK") {
				std::cout << "APPPLLLEEEESSSS" << std::endl;
			}

		break;
		case '2':
			std::cout << "login" << std::endl;
		break;
		default:
			std::cout << "Sorry, I dont understand that" << std::endl;
		break;
	}

	//test::Reg reg1;

	//std::fstream newRead("test.proto", std::ios::in | std::ios::binary);
	//if (!reg1.ParseFromIstream(&newRead)) {
	//	std::cout << "1" << std::endl;
	//}

	//const test::Reg& reg2 = reg1;

	//std::cout << "USERNAME IN PROTO FILE: " << reg2.username() << std::endl;
	//std::cout << "PASSWORD IN PROTO FILE: " << reg2.password() << std::endl;


}

void GameState::setupPlatforms()
{
	platforms_.clear();

	Platform* platform1 = new Platform();
	platform1->createSmall(world_, b2Vec2(700.f / PPM, 400.f / PPM));

	Platform* platform = new Platform();
	platform->createSmall(world_, b2Vec2(500.f / PPM, 400.f / PPM));

	sf::RectangleShape shape(sf::Vector2f(25.f, 25.f));
	shape.setOrigin(12.5f, 12.5f);
	shape.setFillColor(sf::Color::Green);

	//Ground
	Platform* groundPlatform = new Platform();
	groundPlatform->createGroundPlane(world_, b2Vec2((window_->getSize().x / 2) / PPM, (window_->getSize().y - 34.f) / PPM), window_->getSize().x / PPM, 34.f / PPM);


	groundSF_ = sf::RectangleShape(sf::Vector2f(1280.f, 25.f));
	groundSF_.setOrigin(640.f, 12.5f);
	groundSF_.setFillColor(sf::Color::Red);

	Box groundBox;
	groundBox.init(world_, { 640.f / PPM, 600.f / PPM }, { 1280.f / PPM, 25.f / PPM }, false);

	groundSF_.setPosition(sf::Vector2f(groundBox.getBody()->GetPosition().x * PPM, groundBox.getBody()->GetPosition().y * PPM));

	platforms_.push_back(platform1);
	platforms_.push_back(platform);
	platforms_.push_back(groundPlatform);
}

