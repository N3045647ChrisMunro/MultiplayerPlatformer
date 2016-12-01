#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "Box.h"

class Bullet : public sf::Drawable
{
public:
	Bullet() = default;

	Bullet(b2World *world);
	~Bullet();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void spawnBullet(sf::Vector2f spawnPos, sf::Vector2f targetPos, bool isFacingRight);

	sf::Sprite getSprite() const { return bSprite_; };

	bool isActive() const { return isActive_; };
	void setActive(bool newState);
	void setPosition(sf::Vector2f position);
	void setTargetPosition(sf::Vector2f targetPos);


	void update(float deltaTime);

private:

	bool isActive_{ false };

	float speed_{ 2000.0f };

	Box collisionBox_;

	b2Vec2 velocity_;
	b2World *world_;

	sf::Vector2f position_{ 0, 0 };
	sf::Vector2f targetPosition_{ 0, 0 };

	sf::Vector2f dimensions_{ 0, 0 };

	sf::Sprite bSprite_;
	sf::Texture bTexture_;
	sf::Texture bHitTexture_;
	sf::RectangleShape shape_;


};

