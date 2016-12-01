#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include "Box.h"

class Platform : public sf::Drawable
{
public:
	Platform();
	~Platform();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	void createSmall(b2World *world, const b2Vec2 &position);

	const Box& getCollisionBox() const { return platformCollisonBox_; }
	void setPosition(const sf::Vector2f pos);
	sf::Vector2f getPosition() const { return position_; }

private:

	Box platformCollisonBox_;

	sf::Vector2f position_{ 0, 0 };
	sf::Vector2f dimensions_{ 0, 0 };

	sf::Texture texture_;
	sf::Sprite pfSprite_;

	sf::RectangleShape shape_;

};

