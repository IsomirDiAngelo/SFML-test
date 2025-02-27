#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include "../util/globalConstants.h"
#include "../util/collisionTable.h"
#include <iostream>

using namespace std;

class Tile {
private:
    RectangleShape hitbox;

    int x;
    int y;
    int tileType;
    bool solid;

public:
    Tile(int x, int y, int tileType);
    Tile();
    void setX(int x);
    void setY(int y);
    int getX() const;
    int getY() const;
    int getTileType() const;
    bool isSolid() const;
    sf::RectangleShape& getHitbox();
};

#endif