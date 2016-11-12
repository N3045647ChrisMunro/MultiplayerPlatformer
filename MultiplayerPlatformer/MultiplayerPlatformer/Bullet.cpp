#include "Bullet.h"
#include <iostream>
#include <math.h>


Bullet::Bullet()
{
	if (!bTexture_.loadFromFile("Resources/purpleLaser.png"))
		std::cerr << "Bullet Texture Load Error" << std::endl;

	if (!bHitTexture_.loadFromFile("Resources/laserPurpleDot.png"))
		std::cerr << "Bullet Hit Texture Load Error" << std::endl;

	bSprite_.setTexture(bTexture_);
	bSprite_.setOrigin(17.5f, 7.5f);
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

}

void Bullet::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(bSprite_, states);
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

void Bullet::update()
{
	if (isActive_) {

		const float distance = sqrt((targetPosition_.x - position_.x) * (targetPosition_.x - position_.x) +
							   (targetPosition_.y - position_.y) * (targetPosition_.y - position_.y));

		const float vX = speed_ * (targetPosition_.x - position_.x) / distance;
		const float vY = speed_ * (targetPosition_.y - position_.y) / distance;

		position_.x += vX;
		position_.y += vY;


		bSprite_.setPosition(position_.x, position_.y);

		const float cX = fabsf(targetPosition_.x - position_.x);
		const float cY = fabsf(targetPosition_.y - position_.y);

		if (cX < 10 && cY < 10) {
			bSprite_.setTexture(bHitTexture_);
			isActive_ = false;

		}
	}
}