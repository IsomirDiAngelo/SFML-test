#include "tile.h"

Tile::Tile(int x, int y, int tileType) {
    this->x = x;
    this->y = y;
    this->tileType = tileType;
    this->solid = collisionTable[tileType];
    this->dangerous = dangerTable[tileType];
    if (dangerous) {
        this->hitbox = RectangleShape({TILE_SIZE.x, TILE_SIZE.y - 4});
        this->hitbox.setPosition({(float) x * TILE_SIZE.x, (float) y * TILE_SIZE.y + 4});  
    } else if (leavesTiles.find(tileType) != leavesTiles.end()) {
        this->hitbox = RectangleShape({TILE_SIZE.x - 4, TILE_SIZE.y - 4});
        this->hitbox.setPosition({(float) x * TILE_SIZE.x + 4, (float) y * TILE_SIZE.y + 4}); 
    } else if (branchesTiles.find(tileType) != branchesTiles.end()) {
        this->hitbox = RectangleShape({TILE_SIZE.x, TILE_SIZE.y - 4});
        this->hitbox.setPosition({(float) x * TILE_SIZE.x, (float) y * TILE_SIZE.y});
    } else {
        this->hitbox = RectangleShape({TILE_SIZE.x, TILE_SIZE.y});
        this->hitbox.setPosition({(float) x * TILE_SIZE.x, (float) y * TILE_SIZE.y});   
    }
    this->hitbox.setFillColor(Color::Transparent); // Invisible
    
    if ((solid || dangerous) && DEBUG) {
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

bool Tile::isDangerous() const {
    return dangerous;
}

sf::RectangleShape& Tile::getHitbox()  {
    return hitbox;
}