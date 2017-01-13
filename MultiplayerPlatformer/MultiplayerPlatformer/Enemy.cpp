#include "Enemy.h"

#define PPM 30.f

Enemy::Enemy()
{
}


Enemy::~Enemy()
{
}

void Enemy::createSprite(b2World * world)
{
	enemyPos_ = sf::Vector2f(960, 540);

	enemyCollisionBox_.init(world, b2Vec2(enemyPos_.x / PPM, enemyPos_.y / PPM), b2Vec2(32 / PPM, 96 / PPM), true);

	//Draw Collision Box Outline
	colBox_ = sf::RectangleShape(sf::Vector2f(32, 96));
	colBox_.setOrigin(32 / 2, 96 / 2);
	colBox_.setFillColor(sf::Color::Transparent);
	colBox_.setOutlineColor(sf::Color::Red);
	colBox_.setOutlineThickness(1.0);

}

void Enemy::setAliveStatus(bool state)
{
	isAlive_ = state;
}

void Enemy::setUserName(std::string username)
{
	username_ = username;
}

void Enemy::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	target.draw(colBox_, states);
}

void Enemy::update(float dt)
{
	enemyPos_.x = enemyCollisionBox_.getBody()->GetPosition().x * PPM; //dt * velocity_.x;
	enemyPos_.y = enemyCollisionBox_.getBody()->GetPosition().y * PPM; //dt * velocity_.y;

	colBox_.setPosition(enemyPos_);
}

