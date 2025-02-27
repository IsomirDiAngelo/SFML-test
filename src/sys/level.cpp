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
        int value;
        x = 0;
        while (iss >> value) {
            if (y == -1) { 
                // Read level size from the first line of the file
                if (x == 0) {
                    size.x = value;
                } else if (x == 1) {
                    size.y = value;

                    tiles = vector<vector<Tile>>(size.x, vector<Tile>(size.y, Tile()));

                    // Building 2 triangles per tile
                    vertices.setPrimitiveType(PrimitiveType::Triangles);
                    vertices.resize(size.x * size.y * 6);
                // Read spawn position
                } else if (x == 2) {
                    spawnPosition.x = value;
                } else if (x == 3) {
                    spawnPosition.y = value;
                }
            } else {
                // 25 tiles of dimensions 16*16 per row 
                tiles[x][y] = Tile(x, y, value);

                Vector2i textureCoordinates = {(value % 25) * TILE_SIZE.x, (value / 25) * TILE_SIZE.y};

                // get a pointer to the triangles' vertices of the current tile
                Vertex* triangles = &vertices[(x + y * size.x) * 6];

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
    target.draw(vertices, states);
}

vector<vector<Tile>> Level::getTiles() {
    return tiles;
}

Vector2u Level::getSize() const {
    return size;
}

Vector2u Level::getSpawnPosition() const {
    return spawnPosition;
}

VertexArray& Level::getVertices() {
    return vertices;
}

Texture& Level::getTileset() {
    return tileset;
}