//
//  TCPNetwork.hpp
//  Multiplayer Platformer
//
//  Created by MUNRO, CHRISTOPHER on 04/11/2016.
//  Copyright © 2016 MUNRO, CHRISTOPHER. All rights reserved.
//

#pragma once

#include <SFML/System/Vector2.hpp>

class Box
{
public:
	Box();
	~Box();

	void init();

	const sf::Vector2f& getDimensions() const { return dimensions_; };

private:


	sf::Vector2f dimensions_;
};

