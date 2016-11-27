#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <string>

#include "Box.h"
#include "Bullet.h"

class Player : public sf::Drawable
{
public:
	Player();
	virtual ~Player();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(sf::Event event, float dt);

	void createSprite();

	void setMousePosition(sf::Vector2i mousePos);

	sf::Vector2f getPosition() const { return playerPos_; };

	bool canShoot() const { return canShoot_; };

	const Box& getBox() const { return playerCollisionBox_; }

	void moveRight(float deltaTime);
	void moveLeft(float deltaTime);
	void jump(float deltaTime);
	void fall();
	bool isJumping() const { return jumping_; }
	void setIsJumping(bool state);

private:
	
	bool canJump_{ false };

private:

	Box playerCollisionBox_;

	Bullet bullet_;

	sf::Sprite pSprite_;
	sf::Texture pTexture_;

	sf::Sprite pWeapon_;
	sf::Texture pWeaponTexture_;

	sf::Sprite pShield_;
	sf::Texture pShieldTexture_;

	unsigned int walkTileX_{ 3 };
	unsigned int walkTileY_{ 1 };

	unsigned int actionsTileX_{ 0 };
	unsigned int actionsTileY_{ 0 };

	sf::Clock clock_;

	float moveSpeed_{ 300.0f }; // Player Movement speed
	const int moveStep_{ 30 }; //Amount of pixels the player moves per key press
	const float jumpHeight_{ 75.f }; //Amount of pixels the player jumps

	sf::Vector2f playerPos_{ 0, 0 };
	sf::Vector2i mousePos_{ 0, 0 };
	sf::Vector2i mouseClickPos_{ 0, 0 };

	float weaponRotationAngle_{ 0 };

	bool canShoot_{ true };
	float shootCooldown_{ 0.5f };
	float lastTimeCheck_{ 0.f };

	//Player State Bools
	bool walking_{ true };
	bool action_{ false };
	bool jumping_{ false };

	bool blocking_{ false };
	bool shooting_{ true };

	bool facingLeft_{ false };
	bool facingRight_{ true };

};

