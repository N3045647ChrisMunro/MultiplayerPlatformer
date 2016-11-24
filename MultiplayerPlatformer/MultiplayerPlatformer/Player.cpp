#include "Player.h"
#include <iostream>
#include <math.h>



Player::Player(b2World*  world) : world_(world)
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

	playerPhysicsBox_.init(world_, sf::Vector2f(playerPos_.x, playerPos_.y), sf::Vector2f(72.0f, 97.0f), true);
	velocity_ = playerPhysicsBox_.getBody()->GetLinearVelocity();
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
		velocity_.x = 0;
	}
	//Walk Left
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && walking_ == true) {
		moveLeft(dt);
	}
	//Reset to defualt "standing" position
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::A) {
		walkTileX_ = 3;
		walkTileY_ = 1;

		velocity_.x = 0;
	}

	//Jump
	if (canJump_) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

			//playerPhysicsBox_.getBody()->ApplyLinearImpulseToCenter(b2Vec2(0.0f, -500.f), true);

			velocity_.x = playerPhysicsBox_.getBody()->GetLinearVelocity().x;
			velocity_.y = -10;
			playerPhysicsBox_.getBody()->SetLinearVelocity(velocity_);

			action_ = true;
			walking_ = false;
			jumping_ = true;
			//canJump_ = false;

			actionsTileX_ = 2;
			actionsTileY_ = 2;
		}
	}
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

		velocity_.x = 0;
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



	//std::cout << playerPhysicsBox_.getBody()->GetLinearVelocity().x << ", " << playerPhysicsBox_.getBody()->GetLinearVelocity().y << std::endl;
	//std::cout << velocity_.x << ", " << velocity_.y << std::endl;

	//Loop through all contact points
	auto playerBody = playerPhysicsBox_.getBody();
	for (b2ContactEdge* ce = playerBody->GetContactList(); ce != nullptr; ce = ce->next) {
		b2Contact* c = ce->contact;
		
		if (c->IsTouching()) {
			b2WorldManifold manifold;
			c->GetWorldManifold(&manifold);

			//Check if the contact point is below the player
			for (int i = 0; i < b2_maxManifoldPoints; i++) {
				if (manifold.points[i].y < playerBody->GetPosition().y - playerPhysicsBox_.getDimensions().y - 5.f) {
					canJump_ = true;
				}
				else {
					std::cout << "A" << std::endl;
					canJump_ = false;
				}
			}
		}
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

	//Update physics and player position / physics box position
	velocity_.y = playerPhysicsBox_.getBody()->GetLinearVelocity().y;
	//playerPhysicsBox_.getBody()->SetLinearVelocity(velocity_);
	pSprite_.setPosition(sf::Vector2f(playerPhysicsBox_.getBody()->GetPosition().x, playerPhysicsBox_.getBody()->GetPosition().y));
	playerPos_.x = playerPhysicsBox_.getBody()->GetPosition().x;
	playerPos_.y = playerPhysicsBox_.getBody()->GetPosition().y;

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

void Player::moveRight(float deltaTime)
{
	walking_ = true;
	facingRight_ = true;
	facingLeft_ = false;
	action_ = false;
	jumping_ = false;

	velocity_.x = 10.f;
	velocity_.y = playerPhysicsBox_.getBody()->GetLinearVelocity().y;
	playerPhysicsBox_.getBody()->SetLinearVelocity(velocity_);

	pSprite_.setPosition(playerPos_);

	pSprite_.setScale({ 1.f, 1.f }); //Flip the sprite to face right
	pWeapon_.setScale({ 1.f, 1.f }); //Flip the Weapon Sprite to face right
	pShield_.setScale({ 1.f, 1.f }); //Flip the Shield Sprite to face right

	walkTileX_++;
}

void Player::moveLeft(float deltaTime)
{
	walking_ = true;
	facingRight_ = false;
	facingLeft_ = true;
	action_ = false;
	jumping_ = false;

	velocity_.x = -10.f;
	velocity_.y = playerPhysicsBox_.getBody()->GetLinearVelocity().y;
	playerPhysicsBox_.getBody()->SetLinearVelocity(velocity_);
	
	pSprite_.setPosition(playerPos_);
	
	pSprite_.setScale({ -1.f, 1.f }); //Flip the sprite to face left
	pWeapon_.setScale({ -1.f, 1.f }); //Flip the Weapon Sprite to face left
	pShield_.setScale({ -1.f, 1.f }); //Flip the Shield Sprite to face left

	walkTileX_++;
}
