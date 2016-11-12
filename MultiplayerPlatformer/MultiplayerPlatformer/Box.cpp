#include "Box.h"



Box::Box()
{
}


Box::~Box()
{
}

void Box::init(b2World* world, const sf::Vector2f &position, const sf::Vector2f &dimensions)
{

	dimensions_ = dimensions;

	//Make the Body
	b2BodyDef bodyDef; 
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x, position.y);

	body_ = world->CreateBody(&bodyDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox(dimensions.x / 2, dimensions.y / 2);

	//Make the fixture
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &boxShape;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;

	fixture_ = body_->CreateFixture(&fixtureDef);

}
