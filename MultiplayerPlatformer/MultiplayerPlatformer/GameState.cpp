//
//  GameState.cpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 11/11/2016.
//  Copyright � 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#include "GameState.h"
#include <iostream>
#include <thread>
#include <fstream>
#include <mutex>

//Protobuf
#include "GameDataTCP.pb.h"
#include "GameDataUDP.pb.h"
#include "google/protobuf/compiler/parser.h"
#include "UDPMessenger.hpp"

//Include Game Classes
#include "Player.h"
#include "Enemy.h"
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

		//Creaate box2D physics world
		world_ = new b2World(gravity_);
		world_->SetAllowSleeping(true);

		//Create / Initialize the Player
		player_ = new Player();
		player_->createSprite(world_);

		const int MAX_ENEMIES = 10;
		enemies_.clear();
		/*enemies_.resize(MAX_ENEMIES);

		for (unsigned int i = 0; i < enemies_.size(); i++) {
			enemies_[i] = new Enemy();
			enemies_[i]->createSprite(world_);
		}
         */
        
        udpMessenger_ = new UDPMessenger();
        
        udpNetwork_.setUDPMessenger(udpMessenger_);
		udpNetwork_.setIP_address("25.74.8.120");
		udpNetwork_.setPortNumber(8081);
		udpNetwork_.createSocket();
        
        player_->setUDPMessenger(udpMessenger_);

		startClient();

		//Create game Window
		windowDimensions_ = sf::VideoMode::getFullscreenModes();
		window_ = new sf::RenderWindow(windowDimensions_[10], "SFML Test");


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
	std::thread udp_recvThread(&GameState::recvUDPMessage, this);

	std::thread udp_sendThread(&GameState::sendUDPMessage, this);
	std::thread udp_handleMsg(&GameState::handleUDPMessage, this);

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
			if (event.type == sf::Event::KeyPressed) {
				if (event.key.code == sf::Keyboard::Space) {

					//Send SpaceKey press Msg to server
					//spacePressed();
				}
			}
		}

        const float diffX = fabsf(player_->getPosition().x - tempPos.x);
        const float diffY = fabsf(player_->getPosition().y - tempPos.y);
        
		if (diffX > 1 || diffY > 1) {
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
                    e->update(deltaTime.asSeconds());
					window_->draw(*e);
				}
			}
		}

		window_->draw(*player_);

		window_->display();
	}

	unReg();

	//When the game window closes, join the threads back to the "main" thread
	tcp_recvThread.join();
	udp_recvThread.join();

	udp_sendThread.join();
	udp_handleMsg.join();
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
        
        delete udpMessenger_;

		for (unsigned int i = 0; i < enemies_.size(); i++) {
			delete enemies_[i];
		}
        
        for(auto p : platforms_){
            delete p;
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
    GameDataTCP::DataMessage* dataMsg = new GameDataTCP::DataMessage();
    dataMsg->Clear();
	while (true)
	{
		if (tcpNetwork_->isConnected()) {

			dataMsg = tcpNetwork_->receiveData();

			//Handle the Message
			try {
				//Check to see if the message is aand update about newly registered players
				if (dataMsg->newplayerreg_size() > 0) {

					for (unsigned int i = 0; i < dataMsg->newplayerreg_size(); i++) {

						std::string status = dataMsg->newplayerreg().Get(i).status();
                        //std::string status = dataMsg->newplayerreg(0).status();

						if (status == "NewReg") {
							std::cout << "New Player Registered" << std::endl;
                            
                            //Add a new enemy to the vector array
                            Enemy *newEnemy = new Enemy();
                            newEnemy->setAliveStatus(true);
                            newEnemy->setUserName(dataMsg->newplayerreg().Get(i).username());
                            newEnemy->setSpawnPos(sf::Vector2f(700, 400));
                            newEnemy->createSprite(world_);
                            enemies_.push_back(newEnemy);
                        
                            //delete newEnemy;
							//enemies_[enemiesIDXTracker_]->setAliveStatus(true);
							//enemies_[enemiesIDXTracker_]->setUserName(dataMsg->newplayerreg().Get(i).username());
							enemiesIDXTracker_++;
                            enemyActiveCount_++;
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

			udpNetwork_.receiveData();

		}
		else
			std::cerr << "Error: Connection Lost (UDP)" << std::endl;

	}
}

void GameState::sendUDPMessage()
{
	while (true) {
		std::string messageToSend = udpMessenger_->getSendMessage();

		if (messageToSend.size() > 0) {
			udpNetwork_.sendData(messageToSend);
		}
	}
}

void GameState::handleUDPMessage()
{
    GameDataUDP::DataMessage *dataMsg = new GameDataUDP::DataMessage();

	while (true) {
        
        if(dataMsg != NULL)
            dataMsg->Clear();
        
        
		std::string playerUserName = player_->getUsername();
        std::string who = "";
		dataMsg = udpMessenger_->getRecvMessage();
		if (dataMsg != nullptr) {
            
			//check if the message is a POSITION UPDDATE
			if (dataMsg->has_playerposupdate()) {

				who = dataMsg->playerposupdate().username();
                
				if (who != playerUserName) {

					if (enemyActiveCount_ > 0) {

						for (auto e : enemies_) {
							if (e->isAlive() && e->getUserName() == who) {
                                const float x = dataMsg->playerposupdate().xpos();
                                const float y = dataMsg->playerposupdate().ypos();
                                e->setEnemyPos(sf::Vector2f(x, y));
                                break;
                            }
						}
					}

				}

            }
            //Check if the message is a VELOCITY UPDATE
            if (dataMsg->has_playervelocityupdate()) {
                
                who = dataMsg->playervelocityupdate().username();
                
                if(who != playerUserName){
                    
                    std::cout << who << std::endl;
                    
                    if(enemyActiveCount_ > 0){
                            
                        for(auto e : enemies_){
                                
                            if(e->isAlive() && e->getUserName() == who){
                                
                                b2Vec2 vel;
                                
                                vel.x = dataMsg->playervelocityupdate().xpos();
                                vel.y = dataMsg->playervelocityupdate().ypos();
                                
                                e->setEnemyVelocity(vel);
                                break;
                            }
                            
                        }
                    }
                }
			}
        }//end datamsg
    }//end while
}

void GameState::startClient()
{
	//Set up the network connection to the server
	tcpNetwork_ = new TCPNetwork();
    
    std::cout << "Please Enter Server IP Address" << std::endl;
    std::cout << ">>: ";
    std::string ipIn;
    std::cin >> ipIn;
    
	tcpNetwork_->setIP_address(std::string(ipIn));
    
    #ifdef  __APPLE__
    tcpNetwork_->setPortNumber(8080);
    #elif _WIN32
	tcpNetwork_->setPortNumber(std::string("8080"));
    #endif
    
	tcpNetwork_->createSocket();
    if(!tcpNetwork_->connectToServer()){
        std::cout << "Invalid IP, Please Try again " << std::endl;
        startClient();
    }

	GameDataTCP::DataMessage* dataMsg = new GameDataTCP::DataMessage();
	GameDataTCP::Register* regData = new GameDataTCP::Register();
	GameDataTCP::Login* loginData = new GameDataTCP::Login();
	std::string name, pass, encyrpted;
	char key[] = { 'Q', 'B', 'Y' }; //Encyrption Key;
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
		player_->setUsername(name);

		regData->set_username(name);

		std::cout << sizeof(char) << std::endl;

		for (unsigned int i = 0; i < pass.size(); ++i) {
			encyrpted += pass[i] ^ key[i % sizeof(key) / sizeof(char)];
		}

		regData->set_password(encyrpted);

		dataMsg->set_allocated_register_(regData);

		buff_string = dataMsg->SerializeAsString();

		std::cout << buff_string << std::endl;

		//Send a registration request to the server
		tcpNetwork_->sendData(buff_string);
		dataMsg = tcpNetwork_->receiveData();

		//Handle the message
		if (dataMsg->newplayerreg_size() >= 0) {

			//Check to see wether the registration was successful
			status = dataMsg->newplayerreg().Get(0).status();

			if (status == "Success") {
				if (dataMsg->newplayerreg_size() > 1) {
					for (unsigned int i = 1; i < dataMsg->newplayerreg_size(); i++) {
						status = dataMsg->newplayerreg().Get(i).status();
						if (status == "oldClients") {
                            
                            //Create a new enemy to add to the vector array
                            Enemy *newEnemy = new Enemy();
                            
                            newEnemy->setAliveStatus(true);
                            newEnemy->setUserName(dataMsg->newplayerreg().Get(i).username());
                            newEnemy->setSpawnPos(sf::Vector2f(700, 400));
                            newEnemy->createSprite(world_);
                            enemies_.push_back(newEnemy);
                            
                            //delete newEnemy;
                            
							//enemies_[i - 1]->setAliveStatus(true); // - 1 so we start at the first index
							//enemies_[i - 1]->setUserName(dataMsg->newplayerreg().Get(i).username());
							//enemies_[i - 1]->setEnemyPos(sf::Vector2f(300, 200));
							enemyActiveCount_++;
						}
					}
				}
			}
			else if (status == "Failed") {
				std::cout << "Sorry That Username is Taken" << std::endl;
				startClient();
			}

		}
		break;
	case '2':
		std::cout << "login" << std::endl;
		std::cout << "Username: ";
		std::cin >> name;

		std::cout << "Password: ";
		std::cin >> pass;

		std::cout << std::endl;
		player_->setUsername(name);

		loginData->set_username(name);

		std::cout << sizeof(char) << std::endl;

		for (unsigned int i = 0; i < pass.size(); ++i) {
			encyrpted += pass[i] ^ key[i % sizeof(key) / sizeof(char)];
		}

		loginData->set_password(encyrpted);

		dataMsg->set_allocated_login(loginData);

		buff_string = dataMsg->SerializeAsString();

		std::cout << buff_string << std::endl;

		//Send a registration request to the server
		tcpNetwork_->sendData(buff_string);
		dataMsg = tcpNetwork_->receiveData();
		
		//Handle the message
		if (dataMsg->newplayerreg_size() >= 0) {

			//Check to see wether the registration was successful
			status = dataMsg->newplayerreg().Get(0).status();

			if (status == "Success") {
				if (dataMsg->newplayerreg_size() > 1) {
					for (unsigned int i = 1; i < dataMsg->newplayerreg_size(); i++) {
						status = dataMsg->newplayerreg().Get(i).status();
						if (status == "oldClients") {

							//Create a new enemy to add to the vector array
							Enemy *newEnemy = new Enemy();

							newEnemy->setAliveStatus(true);
							newEnemy->setUserName(dataMsg->newplayerreg().Get(i).username());
							newEnemy->setSpawnPos(sf::Vector2f(700, 400));
							newEnemy->createSprite(world_);
							enemies_.push_back(newEnemy);

							//delete newEnemy;

							//enemies_[i - 1]->setAliveStatus(true); // - 1 so we start at the first index
							//enemies_[i - 1]->setUserName(dataMsg->newplayerreg().Get(i).username());
							//enemies_[i - 1]->setEnemyPos(sf::Vector2f(300, 200));
							enemyActiveCount_++;
						}
					}
				}
			}
			else if (status == "Failed") {
				std::cout << "Sorry! Login Failed" << std::endl;
				startClient();
			}

		}
		break;
	default:
		std::cout << "Sorry, I dont understand that" << std::endl;
		break;
	}

	delete regData;
	delete dataMsg;
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
    
    delete dataMsg;
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
	GameDataUDP::PlayerPositionUpdate* posData = new GameDataUDP::PlayerPositionUpdate();

	posData->set_username(player_->getUsername());
	posData->set_xpos(player_->getPosition().x);
	posData->set_ypos(player_->getPosition().y);
	dataMsg->set_allocated_playerposupdate(posData);

	std::string stringBuff = dataMsg->SerializeAsString();

	udpMessenger_->addMsgToSendQueue(stringBuff);
    
    delete dataMsg;
}


