//
//  TCPNetwork.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 04/11/2016.
//  Copyright � 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

#include <SFML/System/Vector2.hpp>
#include <Box2D/Box2D.h>

class Box
{
public:
	Box();
	~Box();

	void init(b2World* world, const b2Vec2 &position, const b2Vec2 &dimensions, bool dynamic);

	b2Body* getBody() const { return body_; };
	b2Fixture* getFixture() const { return fixture_; };

	const sf::Vector2f& getDimensions() const { return dimensions_; };

private:
    
    b2BodyDef bodyDef_;
	b2Body* body_{ nullptr };
	b2Fixture* fixture_{ nullptr };

	sf::Vector2f dimensions_;
};

