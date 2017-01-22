#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System/Vector2.hpp>
#include <Box2D/Box2D.h>
#include <string>

#include "Box.h"

class Enemy : public sf::Drawable
{
public:
    Enemy();
    ~Enemy();
    
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void update(float dt);
    
    void createSprite(b2World *world);
    
    void setAliveStatus(bool state);
    bool isAlive() const { return isAlive_; }
    
    void setUserName(std::string username);
    std::string getUserName() const { return username_; }
    
    void setEnemyPos(sf::Vector2f pos);
    void setSpawnPos(sf::Vector2f pos);
    void setEnemyVelocity(b2Vec2 vel);
    
    sf::Vector2f getPosition() const { return enemyPos_; }
    
private:
    
    bool isAlive_{ false };
    
    Box enemyCollisionBox_;
    sf::Sprite eSprite_;
    sf::Texture eTexture_;
    sf::RectangleShape colBox_;
    
    sf::Vector2f enemyPos_{-150, -150};
    b2Vec2 enemyNewVelocity_{ 0, 0 };
    b2Vec2 enemyOldVelocity_{ 0, 0 };
    
    bool velocityChanged_{false};
    
    std::string username_{ "" };
};

