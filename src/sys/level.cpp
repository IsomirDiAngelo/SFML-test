#include <SFML/Graphics.hpp>
#include <fstream>
#include "level.h"

Level::Level() {}

/**
 * Class constructor
 * Load a .lvl file and a tileset and build the level object from it
 */
Level::Level(string levelFilename, string tilesetFilename) {
    if (!tileset.loadFromFile(tilesetFilename)) {
        throw runtime_error("Failed to load tileset");
    }

    ifstream file(levelFilename);
    if (!file.is_open()) {
        throw runtime_error("Failed to open level file");
    }

    string line;

    unsigned int x = 0; 
    unsigned int y = -1;

    while (getline(file, line)) {
        istringstream iss(line);
        string stringValue;
        MapEntityType met = MapEntityType::_NULL;
        Vector2f entitySpawnPosition;
        x = 0;
        while (iss >> stringValue) {
            if (y == -1) { 
                int value = stoi(stringValue);
                // Read level size from the first line of the file
                if (x == 0) {
                    size.x = value;
                } else if (x == 1) {
                    size.y = value;

                    tiles = vector<vector<Tile>>(size.x, vector<Tile>(size.y, Tile()));
                    entities = {};

                    // Building 2 triangles per tile
                    mainLayerVertices.setPrimitiveType(PrimitiveType::Triangles);
                    mainLayerVertices.resize(size.x * size.y * 6);

                    backgroundLayerVertices.setPrimitiveType(PrimitiveType::Triangles);
                    backgroundLayerVertices.resize(size.x * size.y * 6);
                // Read spawn position
                } else if (x == 2) {
                    spawnPosition.x = value;
                } else if (x == 3) {
                    spawnPosition.y = value;
                }
            } else if (y < size.y) {
                int value = stoi(stringValue);
                // 25 tiles of dimensions 16*16 per row 
                tiles[x][y] = Tile(x, y, value);

                Vector2u textureCoordinates = {(value % 25) * TILE_SIZE.x, (value / 25) * TILE_SIZE.y};

                // get a pointer to the triangles' mainLayerVertices of the current tile
                Vertex* triangles = &mainLayerVertices[(x + y * size.x) * 6];

                triangles[0].position = Vector2f(x * TILE_SIZE.x, y * TILE_SIZE.y);
                triangles[1].position = Vector2f((x + 1) * TILE_SIZE.x, y * TILE_SIZE.y);
                triangles[2].position = Vector2f(x * TILE_SIZE.x, (y + 1) * TILE_SIZE.y);
                triangles[3].position = Vector2f(x * TILE_SIZE.x, (y + 1) * TILE_SIZE.y);
                triangles[4].position = Vector2f((x + 1) * TILE_SIZE.x, y * TILE_SIZE.y);
                triangles[5].position = Vector2f((x + 1) * TILE_SIZE.x, (y + 1) * TILE_SIZE.y);

                triangles[0].texCoords = Vector2f(textureCoordinates.x, textureCoordinates.y);
                triangles[1].texCoords = Vector2f(textureCoordinates.x + TILE_SIZE.x, textureCoordinates.y);
                triangles[2].texCoords = Vector2f(textureCoordinates.x, textureCoordinates.y + TILE_SIZE.y);
                triangles[3].texCoords = Vector2f(textureCoordinates.x, textureCoordinates.y + TILE_SIZE.y);
                triangles[4].texCoords = Vector2f(textureCoordinates.x + TILE_SIZE.x, textureCoordinates.y);
                triangles[5].texCoords = Vector2f(textureCoordinates.x + TILE_SIZE.x, textureCoordinates.y + TILE_SIZE.y);
            } else if (y < size.y * 2) { // Background layer
                int value = stoi(stringValue);
                int backgroundX = x;
                int backgroundY = y - size.y;

                Vector2u textureCoordinates = {(value % 25) * TILE_SIZE.x, (value / 25) * TILE_SIZE.y};

                // get a pointer to the triangles' backgroundLayerVertices of the current tile
                Vertex* triangles = &backgroundLayerVertices[(backgroundX + backgroundY * size.x) * 6];

                triangles[0].position = Vector2f(backgroundX * TILE_SIZE.x, backgroundY * TILE_SIZE.y);
                triangles[1].position = Vector2f((backgroundX + 1) * TILE_SIZE.x, backgroundY * TILE_SIZE.y);
                triangles[2].position = Vector2f(backgroundX * TILE_SIZE.x, (backgroundY + 1) * TILE_SIZE.y);
                triangles[3].position = Vector2f(backgroundX * TILE_SIZE.x, (backgroundY + 1) * TILE_SIZE.y);
                triangles[4].position = Vector2f((backgroundX + 1) * TILE_SIZE.x, backgroundY * TILE_SIZE.y);
                triangles[5].position = Vector2f((backgroundX + 1) * TILE_SIZE.x, (backgroundY + 1) * TILE_SIZE.y);

                triangles[0].texCoords = Vector2f(textureCoordinates.x, textureCoordinates.y);
                triangles[1].texCoords = Vector2f(textureCoordinates.x + TILE_SIZE.x, textureCoordinates.y);
                triangles[2].texCoords = Vector2f(textureCoordinates.x, textureCoordinates.y + TILE_SIZE.y);
                triangles[3].texCoords = Vector2f(textureCoordinates.x, textureCoordinates.y + TILE_SIZE.y);
                triangles[4].texCoords = Vector2f(textureCoordinates.x + TILE_SIZE.x, textureCoordinates.y);
                triangles[5].texCoords = Vector2f(textureCoordinates.x + TILE_SIZE.x, textureCoordinates.y + TILE_SIZE.y);
            } else { // Entities
                if (x == 0) {
                    if (!stringValue.compare("TA")) {
                        met = MapEntityType::TUTORIAL_ARROW;
                    } else if (!stringValue.compare("SF")) {
                        met = MapEntityType::SACRED_FRUIT;
                    }
                } else if (x == 1) {
                    int value = stoi(stringValue);
                    entitySpawnPosition.x = value;
                } else if (x == 2) {
                    int value = stoi(stringValue);
                    entitySpawnPosition.y = value; 
                    if (met == MapEntityType::TUTORIAL_ARROW) {
                        getline(iss, line);
                        MapEntity* entity = new MapEntity(met, entitySpawnPosition, line);
                        entities.push_back(entity);
                    } else {
                        MapEntity* entity = new MapEntity(met, entitySpawnPosition);
                        entities.push_back(entity);
                    }                 
                }
            }
            x++;
        }
        y++;
    }

    if (!backgroundTexture.loadFromFile(BACKGROUND_SPRITE_FILENAME)) {
        throw runtime_error("Failed to load background");
    }
}

/**
 * Override draw method from sf::Drawable
 */
void Level::draw(RenderTarget& target, RenderStates states) const {
    // Draw the background first
    Sprite background = Sprite(backgroundTexture);
    background.setScale({5, 5});
    target.draw(background);

    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = &tileset;

    // draw the vertex array
    target.draw(backgroundLayerVertices, states);
    target.draw(mainLayerVertices, states);
}

void Level::updateEntities(Player& player, RenderWindow& window, bool& gameFinished) {
    // for (int i = 0; i < entities.size(); i++) {
    //     entities[i].update(player, window, gameFinished);
    //     window.draw(entities[i].getSprite());
    // } 
} 

vector<vector<Tile>> Level::getTiles() {
    return tiles;
}

// array<MapEntity*, 10>& Level::getEntities() {
//     return entities;
// }

Vector2u Level::getSize() const {
    return size;
}

Vector2u Level::getSpawnPosition() const {
    return spawnPosition;
}

VertexArray& Level::getMainLayerVertices() {
    return mainLayerVertices;
}

Texture& Level::getTileset() {
    return tileset;
}