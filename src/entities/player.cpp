#include "player.h"

Player::Player(Vector2f spawnPosition) : sprite(texture) {
    texture = Texture(PLAYER_SPRITE_FILENAME, false, IntRect({0, 0}, PLAYER_SPRITE_SIZE));
    sprite = Sprite(texture);
    sprite.setPosition(spawnPosition); 
    
    health = 100;
    hitbox = RectangleShape(HITBOX_SIZE);
    hitbox.setFillColor(Color::Transparent); // Invisible

    if (DEBUG) {
        hitbox.setOutlineThickness(-1);
        hitbox.setOutlineColor(Color::Red);
    }

    updateHitbox();
    
    speed = {0.0f, 10.0f};
    acceleration = {300.0f, 1000.0f};
    friction = {600.0f, 0.0f};

    groundedState = false;
    dyingState = false;
}

RectangleShape& Player::getHitbox() {
    return hitbox;
}

Sprite& Player::getSprite() {
    return sprite;
}

/**
 * Handle player actions, animation states and 
 */
void Player::update(float deltaTime, Level& level, Input& input) {
    // Player is dying
    if (dyingState) {
        if (!animate(deltaTime, 0.2f, 0, SPRITE_OFFSET_DYING, 3, false)) {
            return;
        } else {
            sprite.setOrigin({0, 0});
            sprite.setScale({1, 1});
            sprite.setPosition(Vector2f(level.getSpawnPosition()));
            actionQueue = queue<Action>();
            resetSpeed();
            resetAnimation();
            dyingState = false;
        }
    }

    // Player is landing from a fall
    if (landingState) {
        if (animate(deltaTime, 0.1f, 4 * 32, SPRITE_OFFSET_JUMPING, 4, false)) {
            resetAnimation();
            landingState = false;
        }
    }

    if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
        // Sprite is facing right
        sprite.setOrigin({0, 0});
        sprite.setScale({1, 1});

        // Accelerate right
        if (!(Keyboard::isKeyPressed(Keyboard::Key::Left))) {
            speed.x += acceleration.x * deltaTime;
        } 
        
        // Make the player turn faster if he was moving left before
        if (speed.x < 0) {
            applyFriction(deltaTime, abs(speed.x) / 40);
        }
    } else if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
        // Sprite is facing left
        sprite.setOrigin({(float) frameWidth, 0});
        sprite.setScale({-1, 1});

        // Accelerate left 
        if (!(Keyboard::isKeyPressed(Keyboard::Key::Right))) {
            speed.x -= acceleration.x * deltaTime;
        } 

        // Make the player turn faster if he was moving right before
        if (speed.x > 0) {
            applyFriction(deltaTime, abs(speed.x) / 40);
        }
    } else {
        applyFriction(deltaTime, 1.0f);
    }

    // Jumping
    if (input.isKeyTriggered(Keyboard::Scancode::Space)) {
        if (groundedState) {
            jump();
        } else if (landingState && actionQueue.front() != Action::JUMP) {
            // Buffer jump if we are close to landing
            actionQueue.push(Action::JUMP);
        }
    }

    // Falling
    if (!groundedState) {
        // Increase gravity upon falling
        float accelerationMultiplier;
        if (speed.y < 0) {
            accelerationMultiplier = 1.0f;
            animate(deltaTime, 0.2f, 0, SPRITE_OFFSET_JUMPING, 5, false);
        } else {
            accelerationMultiplier = 1.5f;
            sprite.setTextureRect(IntRect({5 * 32, SPRITE_OFFSET_JUMPING}, {frameWidth, frameHeight}));
        }
        speed.y += accelerationMultiplier * acceleration.y * deltaTime;
    }

    // Running and walking
    if (Keyboard::isKeyPressed(Keyboard::Key::LShift) && groundedState) {
        maxSpeed = MAX_SPEED_RUNNING;
    } else {
        if (!groundedState) {
            // If the player is airbone limit speed.x to its value before jumping or falling
            maxSpeed = max(MAX_SPEED_WALKING, std::min(MAX_SPEED_RUNNING, abs(airboneXSpeedSnapshot)));
        } else {
            maxSpeed = MAX_SPEED_WALKING;
        }
    }

    if (speed.x > maxSpeed) {
        speed.x = maxSpeed;
    } else if (speed.x < -maxSpeed) {
        speed.x = -maxSpeed; 
    }

    if (groundedState && !landingState) {
        if (abs(speed.x) > 0 && abs(speed.x) <= MAX_SPEED_WALKING + 25.0f) {
            animate(deltaTime, 0.1f, 0, SPRITE_OFFSET_WALKING, WALKING_FRAMES, true);
        } else if (abs(speed.x) > MAX_SPEED_WALKING + 25.0f) {
            animate(deltaTime, 0.1f, 0, SPRITE_OFFSET_RUNNING, RUNNING_FRAMES, true);
        } else if (abs(speed.x) == 0) {
            resetAnimation();
            sprite.setTextureRect(IntRect({0, 0}, {frameWidth, frameHeight}));
        }
    }

    updatePosition(deltaTime, speed.x * deltaTime, speed.y * deltaTime, level);
}

/**
 * Update player position v2
 * Handle movements and collision (AABB) pixel by pixel
 * 
 * More robust than v1
 */
void Player::updatePosition(float deltaTime, float dx, float dy, Level& level) {
    Vector2f remainder = {};

    std::vector<std::vector<Tile>> tiles = level.getTiles();
    Vector2u levelSize = level.getSize();

    auto movePlayer = [&](int& move, int axis) {
        if (move != 0) {
            if (axis == 0) remainder.x -= move;
            if (axis == 1) remainder.y -= move;
            float moveSign = move > 0 ? 1.0f : -1.0f;

            while (move != 0) {
                int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
                int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;

                hitbox.setPosition(hitbox.getPosition() + Vector2f(axis == 0 ? moveSign : 0, axis == 1 ? moveSign : 0));

                // Check for collisions with level tiles before moving
                for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
                    for (int y = playerGridPositonY - 1; y <= playerGridPositonY + 2; y++) {
                        if (x < 0 || y < 0 || x > levelSize.x || y > levelSize.y) {
                            continue;
                        }
                        if (checkCollision(hitbox, tiles[x][y].getHitbox())) {
                            if (tiles[x][y].isSolid()) {
                                if (axis == 0) speed.x = 0;
                                if (axis == 1) speed.y = 0;
                                return;
                            }
                            if (tiles[x][y].isDangerous()) {
                                kill();
                                return;
                            }
                        }
                    }
                }
                sprite.move({axis == 0 ? moveSign : 0.0f, axis == 1 ? moveSign : 0.0f});
                move -= moveSign;
            }
        }
    };

    // Move Y

    remainder.y += dy;
    int moveY = round(remainder.y);

    movePlayer(moveY, 1);
    updateHitbox();

    // Move X

    remainder.x += dx;
    int moveX = round(remainder.x);

    movePlayer(moveX, 0);
    updateHitbox();

    updateGroundedState(deltaTime, tiles, levelSize);
}

/**
 * Update player grounded state by checking if no tile below us is solid and colliding with the player's feet
 */
void Player::updateGroundedState(float deltaTime, std::vector<std::vector<Tile>>& tiles, Vector2u levelSize) {
    int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
    int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;
    // int y = playerGridPositonY + 2; // The range of tiles below the player is at Y + 2 because the player is 2 tiles tall

    RectangleShape feetHitbox = RectangleShape(hitbox.getSize());
    feetHitbox.setPosition(hitbox.getPosition() + Vector2f({0, 1}));

    RectangleShape landingHitbox = RectangleShape(hitbox.getSize());
    landingHitbox.setPosition(feetHitbox.getPosition() + Vector2f({0, 48}));

    for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
        // Check for tiles below the player's feet (starting from Y + 2)
        for (int y = playerGridPositonY + 2; y <= playerGridPositonY + 4; y++) {
            if (x < 0 || y < 0 || x > levelSize.x || y > levelSize.y) {
                continue;
            }
            // Player is considered landing when he is about to hit the tile below (48 pixels margin)
            if (speed.y >= 0 && !groundedState && tiles[x][y].isSolid() && checkCollision(landingHitbox, tiles[x][y].getHitbox())) {
                resetAnimation();
                landingState = true;
            }
            if (tiles[x][y].isSolid() && checkCollision(feetHitbox, tiles[x][y].getHitbox())) {
                groundedState = true;
                // Jump if the player buffered a jump while landing
                if (!actionQueue.empty() && actionQueue.front() == Action::JUMP) {
                    actionQueue.pop();
                    jump();
                }
                return;
            }
        }
    }

    // Keep in memory the last value of speed.x before jumping or falling off a ledge
    airboneXSpeedSnapshot = speed.x;
    if (groundedState == true) {
        resetAnimation();
    }
    groundedState = false;
}

void Player::updateHitbox() {
    hitbox.setPosition({sprite.getPosition().x + HITBOX_OFFSET.x, sprite.getPosition().y + HITBOX_OFFSET.y});
}

/**
 * Animate player sprite
 * Returns true when the animation is over
 */
bool Player::animate(float deltaTime, float timePerFrame, int offsetX, int offsetY, int totalFrames, bool repeat) {
    animationTimer += deltaTime;
    totalAnimationTimer += deltaTime;
    
    if (totalAnimationTimer >= totalFrames * timePerFrame) {
        totalAnimationTimer = 0.0f;
        if (!repeat) {
            return true;
        }
    }

    if (animationTimer > timePerFrame) {
        currentFrame = (currentFrame + 1) % totalFrames;
        sprite.setTextureRect(IntRect({currentFrame * frameWidth + offsetX, offsetY}, {frameWidth, frameHeight}));
        animationTimer = 0.0f;
    }

    return false;
}

bool Player::checkCollision(RectangleShape& hitboxA, RectangleShape& hitboxB) {
    return hitboxA.getGlobalBounds().findIntersection(hitboxB.getGlobalBounds()).has_value();
}

void Player::applyFriction(float deltaTime, float factor) {
    if (speed.x > 0) {
        speed.x -= factor * friction.x * deltaTime;
        if (speed.x < 0) {
            speed.x = 0;
        }
    } else if (speed.x < 0) {
        speed.x += factor * friction.x * deltaTime;
        if (speed.x > 0) {
            speed.x = 0;
        }
    }
}

void Player::resetSpeed() {
    speed = Vector2f{0, 0};
}

void Player::resetAnimation() {
    currentFrame = 0;
    animationTimer = 0.0f;
    totalAnimationTimer = 0.0f;
}

void Player::jump() {
    speed.y = -375.0f;
}

void Player::kill() {
    resetAnimation();
    dyingState = true;
}
