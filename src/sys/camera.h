#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include "../entities/player.h"
#include "level.h"

using namespace sf;

class Camera {
    private:
        View view;
        Vector2f offset;

    public:
        Camera();
        Camera(Vector2u size);
        void update(Vector2f playerPosition, Vector2u levelSize);
        View& getView();
        Vector2f& getOffset() const;
        
};

#endif