#ifndef PLAYER_H
#define PLAYER_H

class Level;

#include <SFML/Graphics.hpp>
#include <iostream>
#include <math.h>
#include <queue>
#include "../sys/tile.h"
#include "../sys/level.h"
#include "../sys/input.h"
#include "../util/action.h"

#define PLAYER_SPRITE_FILENAME "assets/entities/hooded protagonist penzilla.png"

constexpr Vector2i PLAYER_SPRITE_SIZE {256, 320};

constexpr Vector2f HITBOX_OFFSET {10, 4};
constexpr Vector2f HITBOX_SIZE {10, 28};

#define SPRITE_OFFSET_WALKING 64
#define SPRITE_OFFSET_RUNNING 96
#define SPRITE_OFFSET_JUMPING 5 * 32
#define SPRITE_OFFSET_DYING 6 * 32

#define WALKING_FRAMES 4
#define RUNNING_FRAMES 8

#define MAX_SPEED_WALKING 125.0f
#define MAX_SPEED_RUNNING 200.0f
#define DASHING_SPEED 600.0f


class Player {
    private:
        int health;
        RectangleShape hitbox;
        Texture texture;
        Sprite sprite;
        Vector2f speed;
        Vector2f acceleration;
        Vector2f friction;
    
        float maxSpeed = MAX_SPEED_WALKING;
        float airboneXSpeedSnapshot; 
        int direction = 1; // 1: right -1: left

        int currentFrame = 0;
        int frameWidth = 32;
        int frameHeight = 32;
        float animationTimer = 0.0f;
        float totalAnimationTimer = 0.0f;
    
        bool groundedState = false;
        bool dyingState = false;
        bool landingState = false;
        bool dashingState = false;
        bool canDash = true;

        queue<Action> actionQueue;

        void updateHitbox();
        void applyFriction(float deltaTime, float factor);
        void updateGroundedState(float deltaTime, std::vector<std::vector<Tile>>& tiles, Vector2u levelSize);
        bool animate(float deltaTime, float timePerFrame, int offsetX, int offsetY, int totalFrames, bool repeat);

    public:
        Player(Vector2f spawnPosition);
        RectangleShape& getHitbox();
        Sprite& getSprite();
        void update(float deltaTime, Level& level, Input& input);
        void updatePosition(float deltaTime, float dx, float dy, Level& level);
        void resetSpeed();
        void resetAnimation();
        void faceRight();
        void faceLeft();
        void kill();
        void jump();
        void dash();
        bool checkCollision(RectangleShape& hitboxA, RectangleShape& hitboxB); // This one should be elsewhere probably
        
};

#endif