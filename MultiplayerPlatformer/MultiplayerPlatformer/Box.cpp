#include "Box.h"

#include <iostream>

#define PPM 30.f //Pixels per meter

Box::Box()
{
}


Box::~Box()
{
}

void Box::init(b2World* world, const b2Vec2 &position, const b2Vec2 &dimensions, bool dynamic)
{
	dimensions_.x = dimensions.x * PPM;
	dimensions_.y = dimensions.y * PPM;

	//Make the Body
	b2BodyDef bodyDef;
	bodyDef.position.Set(position.x, position.y);

	if (dynamic)
		bodyDef.type = b2_dynamicBody;
	else
		bodyDef.type = b2_staticBody;

	body_ = world->CreateBody(&bodyDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(dimensions.x / 2, dimensions.y / 2);

	//Make the fixture
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.0f;

	fixture_ = body_->CreateFixture(&fixtureDef);
}
