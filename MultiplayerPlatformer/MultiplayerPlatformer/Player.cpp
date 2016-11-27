#include "Player.h"
#include <iostream>
#include <math.h>



Player::Player()
{
	//ctor
}

Player::~Player()
{
	//dtor
}

void Player::createSprite()
{
	if (!pTexture_.loadFromFile("Resources/Player.png")) {
		std::cerr << "Player Texture Load Error" << std::endl;
	}

	if (!pWeaponTexture_.loadFromFile("Resources/raygun1.png")) {
		std::cerr << "Player Weapon Texture Load Error" << std::endl;
	}

	if (!pShieldTexture_.loadFromFile("Resources/shieldSilver.png")) {
		std::cerr << "Player Shield Texture Load Error" << std::endl;
	}

	pSprite_.setTextureRect(sf::IntRect(walkTileX_ * 72, walkTileX_ * 97, 72, 97));
	pSprite_.setOrigin(36, 48.5);
	pSprite_.setTexture(pTexture_);

	playerPos_ = sf::Vector2f(200, 200);

	pSprite_.setPosition(playerPos_.x, playerPos_.y);

	pWeapon_.setTexture(pWeaponTexture_);
	pWeapon_.setOrigin(0, 16);
	pWeapon_.setPosition(playerPos_.x + 22, playerPos_.y + 22);

	pShield_.setTexture(pShieldTexture_);
	pShield_.setOrigin(0, 35);
	pShield_.setPosition(playerPos_.x - 5, playerPos_.y + 15);
}

void Player::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(pSprite_, states);

	if (shooting_) {
		target.draw(pWeapon_, states);
		if (bullet_.isActive())
			target.draw(bullet_.getSprite(), states);
	}
	else if (blocking_)
		target.draw(pShield_, states);
}

void Player::update(sf::Event event, float dt)
{
	//Shoot
	if (canShoot_ == true && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		canShoot_ = false;

		//Check to see which direction the player is facing, so the bullet spawn offset position can be set
		if(facingRight_ == true && facingLeft_ == false)
			bullet_.spawnBullet((sf::Vector2f)pSprite_.getPosition(), (sf::Vector2f)mousePos_, true);
		else
			bullet_.spawnBullet((sf::Vector2f)pSprite_.getPosition(), (sf::Vector2f)mousePos_, false);
	}

	bullet_.update(dt);
	//Reset the cooldown
	float timeNow = clock_.getElapsedTime().asSeconds();

	if (canShoot_ == false) {
		if (timeNow - lastTimeCheck_ > shootCooldown_) {
			lastTimeCheck_ = timeNow;

			canShoot_ = true;
		}
		else
			canShoot_ = false;
	}


	//Walk Right
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && walking_ == true) {
		moveRight(dt);
	}
	//Reset to defualt "standing" position
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::D) {
		walkTileX_ = 3;
		walkTileY_ = 1;
	}
	//Walk Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && walking_ == true) {
		moveLeft(dt);
	}
	//Reset to defualt "standing" position
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
		walkTileX_ = 3;
		walkTileY_ = 1;
	}
	//Jump
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		jump(dt);
	}
	//Reset to defualt "standing" position
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Space) {
		action_ = false;
		walking_ = true;
		jumping_ = false;

		walkTileX_ = 3;
		walkTileY_ = 1;
	}
	//Crouch/Block, disable movement
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		action_ = true;
		walking_ = false;
		jumping_ = false;

		shooting_ = false;
		blocking_ = true;

		actionsTileX_ = 0;
		actionsTileY_ = 2;
	}
	//When Key is released allow movement again.
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::S) {
		action_ = false;
		walking_ = true;
		jumping_ = false;

		shooting_ = true;
		blocking_ = false;
	}

	if (walkTileX_ > 3) {
		walkTileX_ = 0;
		walkTileY_++;
	}

	if (walkTileY_ > 1) {
		walkTileY_ = 0;
	}

	//Update Weaapon rotation based on mouse position
	if (mousePos_.x > pWeapon_.getPosition().x) {

		float Dx = mousePos_.x - pWeapon_.getPosition().x;
		float Dy = mousePos_.y - pWeapon_.getPosition().y;

		weaponRotationAngle_ = (float)((atan2(Dy, Dx)) * 180 / 3.14159265);
	}
	else if (mousePos_.x < pWeapon_.getPosition().x) {

		float Dx = pWeapon_.getPosition().x - mousePos_.x;
		float Dy = pWeapon_.getPosition().y - mousePos_.y;

		weaponRotationAngle_ = (float)((atan2(Dy, Dx)) * 180 / 3.14159265);
	}

	//Cap the max and min rotation amount
	if (weaponRotationAngle_ > 35)
		weaponRotationAngle_ = 35;
	else if (weaponRotationAngle_ < -35)
		weaponRotationAngle_ = -35;

	//Apply The rotation to the weapon
	pWeapon_.setRotation(weaponRotationAngle_);
	playerPos_ = pSprite_.getPosition();

	//Set Weapon position
	if (facingRight_)
		pWeapon_.setPosition(playerPos_.x + 22, playerPos_.y + 22);
	else if (facingLeft_)
		pWeapon_.setPosition(playerPos_.x - 22, playerPos_.y + 22);

	//Update the shield position
	pShield_.setPosition(pSprite_.getPosition().x - 5, pSprite_.getPosition().y + 20);

	if (walking_)
		pSprite_.setTextureRect(sf::IntRect(walkTileX_ * 72, walkTileY_ * 97, 72, 97));
	else
		pSprite_.setTextureRect(sf::IntRect(actionsTileX_ * 72, actionsTileY_ * 97, 72, 97));

}

void Player::setMousePosition(sf::Vector2i mousePos)
{
	mousePos_ = mousePos;
}

void Player::jump(float deltaTime)
{
	jumping_ = true;
	action_ = true;
	walking_ = false;

	actionsTileX_ = 2;
	actionsTileY_ = 2;

	//pSprite_.move(0, -75.0f);

	sf::Vector2f targetPosition = sf::Vector2f(playerPos_.x, playerPos_.y - jumpHeight_);

	const float distance = sqrtf((targetPosition.x - playerPos_.x) * (targetPosition.x - playerPos_.x) +
								(targetPosition.y - playerPos_.y) * (targetPosition.y - playerPos_.y));

	const float vX = moveSpeed_ * (targetPosition.x - playerPos_.x) / distance;
	const float vY = moveSpeed_ * (targetPosition.y - playerPos_.y) / distance;

	playerPos_.x += vX * deltaTime;
	playerPos_.y += vY * deltaTime;

	pSprite_.setPosition(playerPos_);
}

void Player::fall()
{
	pSprite_.move(0, 0.3);
}

void Player::setIsJumping(bool state)
{
	jumping_ = state;
}

void Player::moveRight(float deltaTime)
{
	walking_ = true;
	facingRight_ = true;
	facingLeft_ = false;
	action_ = false;
	jumping_ = false;

	sf::Vector2f targetPosition = sf::Vector2f(playerPos_.x + moveStep_, playerPos_.y);

	const float distance = sqrtf((targetPosition.x - playerPos_.x) * (targetPosition.x - playerPos_.x) +
								(targetPosition.y - playerPos_.y) * (targetPosition.y - playerPos_.y));

	const float vX = moveSpeed_ * (targetPosition.x - playerPos_.x) / distance;
	const float vY = moveSpeed_ * (targetPosition.y - playerPos_.y) / distance;

	playerPos_.x += vX * deltaTime;
	playerPos_.y += vY * deltaTime;

	pSprite_.setPosition(playerPos_);

	pSprite_.setScale({ 1.f, 1.f }); //Flip the sprite to face right
	pWeapon_.setScale({ 1.f, 1.f }); //Flip the Weapon Sprite to face right
	pShield_.setScale({ 1.f, 1.f }); //Flip the Shield Sprite to face right

	//walkTileX_++;
}

void Player::moveLeft(float deltaTime)
{
	walking_ = true;
	facingRight_ = false;
	facingLeft_ = true;
	action_ = false;
	jumping_ = false;

	sf::Vector2f targetPosition = sf::Vector2f(playerPos_.x - moveStep_, playerPos_.y);

	const float distance = sqrtf((targetPosition.x - playerPos_.x) * (targetPosition.x - playerPos_.x) +
								(targetPosition.y - playerPos_.y) * (targetPosition.y - playerPos_.y));

	const float vX = moveSpeed_ * (targetPosition.x - playerPos_.x) / distance;
	const float vY = moveSpeed_ * (targetPosition.y - playerPos_.y) / distance;

	playerPos_.x += vX * deltaTime;
	playerPos_.y += vY * deltaTime;

	pSprite_.setPosition(playerPos_);
	
	pSprite_.setScale({ -1.f, 1.f }); //Flip the sprite to face left
	pWeapon_.setScale({ -1.f, 1.f }); //Flip the Weapon Sprite to face left
	pShield_.setScale({ -1.f, 1.f }); //Flip the Shield Sprite to face left

	//walkTileX_++;
}
