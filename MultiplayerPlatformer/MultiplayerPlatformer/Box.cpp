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
	bodyDef_.position.Set(position.x, position.y);

	if (dynamic)
		bodyDef_.type = b2_dynamicBody;
	else
		bodyDef_.type = b2_staticBody;

	body_ = world->CreateBody(&bodyDef_);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(dimensions.x / 2, dimensions.y / 2);

	//Make the fixture
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 0.0f;
	fixtureDef.friction = 0.0f;

	fixture_ = body_->CreateFixture(&fixtureDef);
}
