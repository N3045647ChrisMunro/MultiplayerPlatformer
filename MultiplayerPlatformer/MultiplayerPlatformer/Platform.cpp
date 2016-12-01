#include "Platform.h"
#include <iostream>

#define PPM 30.f

Platform::Platform()
{
}


Platform::~Platform()
{
}

void Platform::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(shape_, states);
	target.draw(pfSprite_, states);
}

void Platform::createSmall(b2World *world, const b2Vec2 &position)
{
	if (!texture_.loadFromFile("Resources/platformSmall.png")) {
		std::cerr << "Error: Platform Small Texture Failed to Load" << std::endl;
	}

	pfSprite_.setTexture(texture_);
	dimensions_ = sf::Vector2f(texture_.getSize());
	pfSprite_.setOrigin(dimensions_.x / 2.f, dimensions_.y / 2.f);
	
	platformCollisonBox_.init(world, position, { dimensions_.x / PPM, dimensions_.y / PPM}, false);
	
	shape_ = sf::RectangleShape(sf::Vector2f(dimensions_.x, dimensions_.y));
	shape_.setOrigin(dimensions_.x / 2.f, dimensions_.y / 2);
	shape_.setFillColor(sf::Color::Transparent);
	shape_.setOutlineColor(sf::Color::Red);
	shape_.setOutlineThickness(1.0);

	position_.x = platformCollisonBox_.getBody()->GetPosition().x  * PPM;
	position_.y = platformCollisonBox_.getBody()->GetPosition().y  * PPM;

	shape_.setPosition(position_.x, position_.y);
	pfSprite_.setPosition(position_.x, position_.y);
}

void Platform::setPosition(const sf::Vector2f pos)
{
	position_ = pos;
}

