#include "Bullet.h"
#include <iostream>
#include <math.h>

#define PPM 30.f

Bullet::Bullet(b2World *world)
{
	if (!bTexture_.loadFromFile("Resources/purpleLaser.png"))
		std::cerr << "Bullet Texture Load Error" << std::endl;

	if (!bHitTexture_.loadFromFile("Resources/laserPurpleDot.png"))
		std::cerr << "Bullet Hit Texture Load Error" << std::endl;

	bSprite_.setTexture(bTexture_);
	dimensions_ = sf::Vector2f(bTexture_.getSize());
	bSprite_.setOrigin(dimensions_.x / 2, dimensions_.y / 2);

	shape_ = sf::RectangleShape(sf::Vector2f(dimensions_.x, dimensions_.y));
	shape_.setOrigin(dimensions_.x / 2.f, dimensions_.y / 2);
	shape_.setFillColor(sf::Color::Transparent);
	shape_.setOutlineColor(sf::Color::Red);
	shape_.setOutlineThickness(1.0);

	world_ = world;

}

Bullet::~Bullet()
{
	//dtor
}

void Bullet::spawnBullet(sf::Vector2f spawnPos, sf::Vector2f targetPos, bool isFacingRight)
{
	isActive_ = true;
	position_ = spawnPos;

	if (isFacingRight) {
		position_.x += 30;
		position_.y += 15;
	}
	else
	{
		position_.x -= 30;
		position_.y += 15;
	}

	targetPosition_ = targetPos;

	//Calculater the angle of rotation for the bullet sprite
	const float Dx = position_.x - targetPosition_.x;
	const float Dy = position_.y - targetPosition_.y;

	const float rotationAngle = (atan2(Dy, Dx)) * 180 / 3.14159265;

	bSprite_.setRotation(rotationAngle);

	//Set the texture for sanity reasons
	bSprite_.setTexture(bTexture_);

	//collisionBox_.init(world_, b2Vec2(position_.x / PPM, position_.y / PPM), 
	//				   b2Vec2(dimensions_.x / PPM, dimensions_.y / PPM), false);
	//collisionBox_.getBody()->SetBullet(true);

	//velocity_ = collisionBox_.getBody()->GetLinearVelocity();

}

void Bullet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(bSprite_, states);
	target.draw(shape_, states);
}

void Bullet::update(float deltaTime)
{
	if (isActive_) {

		const float distance = sqrt((targetPosition_.x - position_.x) * (targetPosition_.x - position_.x) +
							   (targetPosition_.y - position_.y) * (targetPosition_.y - position_.y));

		const float vX = speed_ * (targetPosition_.x - position_.x) / distance;
		const float vY = speed_ * (targetPosition_.y - position_.y) / distance;

		position_.x += vX * deltaTime;
		position_.y += vY * deltaTime;

		bSprite_.setPosition(position_.x, position_.y);
		shape_.setPosition(position_.x, position_.y);
		shape_.setRotation(bSprite_.getRotation());
	}
}

void Bullet::setTargetPosition(sf::Vector2f targetPos)
{
	targetPosition_ = targetPos;
}

void Bullet::setPosition(sf::Vector2f position)
{
	position_ = position;
}

void Bullet::setActive(bool newState)
{
	isActive_ = newState;
}