//
//  TCPNetwork.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 04/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

#include <Box2D\Box2D.h>
#include <SFML/System/Vector2.hpp>

class Box
{
public:
	Box();
	~Box();

	void init(b2World* world, const sf::Vector2f &position, const sf::Vector2f &dimensions);

	b2Body* getBody() const { return body_; };
	b2Fixture* getFixture() const { return fixture_; };

	const sf::Vector2f& getDimensions() const { return dimensions_; };

private:

	b2Body* body_{ nullptr };
	b2Fixture* fixture_{ nullptr };

	sf::Vector2f dimensions_;
};

