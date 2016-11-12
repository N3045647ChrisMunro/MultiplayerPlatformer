#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "Bullet.h"
#include <string>
#include <Box2D\Box2D.h>
#include "Box.h"

class Player : public sf::Drawable
{
public:
	Player(b2World* world);
	virtual ~Player();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void update(sf::Event event, float dt);

	void createSprite();

	void setMousePosition(sf::Vector2i mousePos);

	sf::Vector2f getPosition() const { return playerPos_; };

	bool canShoot() const { return canShoot_; };

private:

	void moveRight();
	void moveLeft();

private:

	b2World* world_{ nullptr };
	Box playerPhysicsBox_;

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

	float moveSpeed_{ 0.1f }; // Player Movement speed
	const int moveStep_{ 30 }; //Amount of pixels the player moves per key press

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

