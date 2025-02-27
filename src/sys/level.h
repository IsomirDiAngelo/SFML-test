#ifndef LEVEL_H
#define LEVEL_H

#include "tile.h"
#include <iostream>

#define BACKGROUND_SPRITE_FILENAME "assets/backgrounds/sky.png"

using namespace std;
using namespace sf;

class Level : public Drawable, public Transformable {
    private:
        vector<vector<Tile>> tiles;
        VertexArray vertices; 
        Texture tileset;
        Texture backgroundTexture;
        Vector2u size;
        Vector2u spawnPosition;
        virtual void draw(RenderTarget& target, RenderStates states) const override;

    public:
        Level();
        Level(string levelFilename, string tilesetFilename);
        vector<vector<Tile>> getTiles();
        Vector2u getSize() const;
        Vector2u getSpawnPosition() const;
        VertexArray& getVertices();
        Texture& getTileset();
        Sprite& getBackground();
};

#endif