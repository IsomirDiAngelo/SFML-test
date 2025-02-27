#include "tile.h"

Tile::Tile(int x, int y, int tileType) {
    this->x = x;
    this->y = y;
    this->hitbox = RectangleShape({TILE_SIZE.x, TILE_SIZE.y});
    this->hitbox.setPosition({x * TILE_SIZE.x, y * TILE_SIZE.y});
    this->hitbox.setFillColor(Color::Transparent); // Invisible
    this->tileType = tileType;
    this->solid = collisionTable[tileType];
    
    if (solid && DEBUG) {
        this->hitbox.setOutlineThickness(-1);
        this->hitbox.setOutlineColor(sf::Color::Red);
    }
}

// Default empty constructor, only to be used to initialize a vector of Tile objects
Tile::Tile() {}

void Tile::setX(int x) {
    this->x = x;
}

void Tile::setY(int y) {
    this->y = y;
}

int Tile::getX() const {
    return x;
}

int Tile::getY() const {
    return y;
}

int Tile::getTileType() const {
    return tileType;
}

bool Tile::isSolid() const {
    return solid;
}

sf::RectangleShape& Tile::getHitbox()  {
    return hitbox;
}