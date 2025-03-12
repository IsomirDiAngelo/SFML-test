#include "player.h"

Player::Player(Vector2f spawnPosition) : sprite(texture) {
    texture = Texture(PLAYER_SPRITE_FILENAME, false, IntRect({0, 0}, PLAYER_SPRITE_SIZE));
    sprite = Sprite(texture);
    sprite.setPosition(spawnPosition); 
    sprite.setTextureRect(IntRect({0, 0}, {frameWidth, frameHeight}));
    
    health = 100;
    hitbox = RectangleShape(HITBOX_SIZE);
    hitbox.setFillColor(Color::Transparent); // Invisible

    if (DEBUG) {
        hitbox.setOutlineThickness(-1);
        hitbox.setOutlineColor(Color::Red);
    }

    updateHitbox();
    
    speed = {0.0f, 0.0f};
    acceleration = {300.0f, 1000.0f};
    friction = {600.0f, 0.0f};

    groundedState = true;
    dyingState = false;
    dashingState = false;
    jumpingState = false;
    canDash = true;
    colliding = true;
}

RectangleShape& Player::getHitbox() {
    return hitbox;
}

Sprite& Player::getSprite() {
    return sprite;
}

/**
 * Handle player actions, animation states
 */
void Player::update(float deltaTime, Clock& globalClock, Level& level, Input& input) {
    // cout << sprite.getPosition().x << "," << sprite.getPosition().y << endl;

    // Player is dying
    if (dyingState) {
        if (animate(deltaTime, 0.2f, 0, SPRITE_OFFSET_DYING, 3, false)) {
            dyingState = false;
            faceRight();
            globalClock.restart();
            sprite.setPosition(Vector2f(level.getSpawnPosition()));
            sprite.setTextureRect(IntRect({0, 0}, {frameWidth, frameHeight}));
            // actionQueue = queue<Action>();
            resetSpeed();
            resetAnimation();
        } else {
            return;
        }
    }

    // Player is landing from a fall
    if (landingState && !jumpingState) {
        if (animate(deltaTime, 0.04f, 4 * 32, SPRITE_OFFSET_JUMPING, 4, false)) {
            resetAnimation();
            landingState = false;
        }
    }

    // Player is dashing
    if (dashingState) {
        speed.y = 0;
        animate(deltaTime, 0.01f, 0, 9 * 32, 8, true);
        applyFriction(deltaTime, 4.0f);
        if (abs(speed.x) <= MAX_SPEED_RUNNING) {
            dashingState = false;
        }
    } 

    if (groundedState && !landingState && !dashingState) {
        if (abs(speed.x) > 0 && abs(speed.x) <= MAX_SPEED_WALKING + 25.0f) {
            animate(deltaTime, 0.1f, 0, SPRITE_OFFSET_WALKING, WALKING_FRAMES, true);
        } else if (abs(speed.x) > MAX_SPEED_WALKING + 25.0f) {
            animate(deltaTime, 0.1f, 0, SPRITE_OFFSET_RUNNING, RUNNING_FRAMES, true);
        } else if (abs(speed.x) == 0) {
            resetAnimation();
            sprite.setTextureRect(IntRect({0, 0}, {frameWidth, frameHeight}));
        }
    }
    
    if (!dashingState || speed.y < 0) {
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
            faceRight();
    
            // Accelerate right
            if (!(Keyboard::isKeyPressed(Keyboard::Key::Left))) {
                speed.x += acceleration.x * deltaTime;
            } 
            
            // Make the player turn faster if he was moving left before
            if (speed.x < 0) {
                applyFriction(deltaTime, abs(speed.x) / 40);
            }
        } else if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
            faceLeft();
    
            // Accelerate left 
            if (!(Keyboard::isKeyPressed(Keyboard::Key::Right))) {
                speed.x -= acceleration.x * deltaTime;
            } 
    
            // Make the player turn faster if he was moving right before
            if (speed.x > 0) {
                applyFriction(deltaTime, abs(speed.x) / 40);
            }
        } else if (abs(speed.x) > 0) {
            applyFriction(deltaTime, 1.0f);
        }

        // Finish the jumping animation if the player started jumping
        if (jumpingState) {
            jumpingState = !animate(deltaTime, 0.04f, 0, SPRITE_OFFSET_JUMPING, 6, false);
        }

        // Falling
        if (!groundedState && !dashingState) {
            // Increase gravity upon falling
            float accelerationMultiplier;

            if (speed.y < 0) {
                accelerationMultiplier = 1.0f;
            } else {
                accelerationMultiplier = 1.25f;
            }

            if (input.isKeyReleased(Keyboard::Scancode::Space) && speed.y < -50.0f) {
                speed.y = -50.0f; // Cancel upward momentum if space is released while jumping
            } else if (!dashingState || speed.y < 0) {
                speed.y += accelerationMultiplier * acceleration.y * deltaTime;
            }
        }
        
        if (!dashingState) {
            // Limit speed if the player is not dashing
            if (speed.x > maxSpeed) {
                speed.x = maxSpeed;
            } else if (speed.x < -maxSpeed) {
                speed.x = -maxSpeed; 
            }

            // Dashing
            if (canDash && input.isKeyTriggered(Keyboard::Scancode::A)) {
                dash();
            }
        }

    }

    // Jumping
    if (input.isKeyTriggered(Keyboard::Scancode::Space)) {
        if (groundedState) {
            jump();
        } else if (landingState && (actionQueue.empty() || actionQueue.front() != Action::JUMP)) {
            actionQueue.push(Action::JUMP);
        }
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

    // cout << speed.x * deltaTime << endl;

    // updatePosition(deltaTime, speed.x * deltaTime, speed.y * deltaTime, level);
    // Vector2f remainder = {abs(speed.x) * deltaTime, abs(speed.y) * deltaTime};
    // Vector2i sign = {speed.x > 0 ? 1 : -1, speed.y > 0 ? 1 : -1};

    updatePosition2({speed.x * deltaTime, speed.y * deltaTime}, deltaTime, level);
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

    // Move the player in axis X (0) or Y (1) and handle collisions if necessary
    auto movePlayer = [&](int& move, int axis) {
        if (move != 0) {
            if (axis == 0) remainder.x -= move;
            if (axis == 1) remainder.y -= move;
            float moveSign = move > 0 ? 1.0f : -1.0f;

            while (move != 0) {
                int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
                int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;

                hitbox.setPosition(hitbox.getPosition() + Vector2f(axis == 0 ? moveSign : 0, axis == 1 ? moveSign : 0));

                // Prevent out of bounds and kill player if he falls down to the bottom
                if (hitbox.getPosition().x < 0 || hitbox.getPosition().x + hitbox.getSize().x > levelSize.x * TILE_SIZE.x 
                    || hitbox.getPosition().y < 0 || hitbox.getPosition().y + hitbox.getSize().y > levelSize.y * TILE_SIZE.y) {
                    if (axis == 0) speed.x = 0;
                    if (axis == 1) {
                        if (move > 0) {
                            kill();
                        } else {
                            speed.y = 0;
                        }
                    }
                    cout << "out of bounds" << endl;
                    return;
                } 

                // Check for collisions with level tiles before moving
                for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
                    for (int y = playerGridPositonY - 1; y <= playerGridPositonY + 2; y++) {
                        if (x < 0 || y < 0 || x >= levelSize.x || y >= levelSize.y) {
                            continue;
                        }
                        if (checkCollision(hitbox, tiles[x][y].getHitbox())) {
                            if (tiles[x][y].isSolid()) {
                                cout << "collision" << endl;
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
                cout << "moving sprite" << endl;
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

void Player::updatePosition2(Vector2f deltaPosition, float deltaTime, Level& level) {

    std::vector<std::vector<Tile>> tiles = level.getTiles();
    Vector2u levelSize = level.getSize();
    
    // if (sign.y == 1 && remainder.y > 0 || sign.y == -1 && remainder.y < 0) {
    //     float distanceY = sign.y;
    //     if (movePlayer(distanceY, 'y', tiles, levelSize)) {
    //         remainder.y -= sign.y * distanceY;
    //     } else {
    //         remainder.y = 0;
    //     }
    // } else if (sign.x == 1 && remainder.x > 0 || sign.x == -1 && remainder.x < 0) {
    //     float distanceX = sign.x;
    //     if (movePlayer(distanceX, 'x', tiles, levelSize)) {
    //         remainder.x -= sign.x * distanceX;
    //     }
    // }

    movePlayer(deltaPosition.y, deltaTime, 'y', tiles, levelSize);
    updateHitbox();

    movePlayer(deltaPosition.x, deltaTime, 'x', tiles, levelSize);
    updateHitbox();

    updateGroundedState(deltaTime, tiles, levelSize);
}

void Player::movePlayer(float distance, float deltaTime, char axis, vector<vector<Tile>>& tiles, Vector2u levelSize) {
    
    bool colliding = false;
    float moved = abs(distance);
    int moveSign = copysign(1.0, distance);

    while (moved > 0) {
        int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
        int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;

        hitbox.setPosition(hitbox.getPosition() + Vector2f(axis == 'x' ? distance * deltaTime : 0, axis == 'y' ? distance * deltaTime : 0));

        // Prevent out of bounds and kill player if he falls down to the bottom
        if (hitbox.getPosition().x < 0 || hitbox.getPosition().x + hitbox.getSize().x > levelSize.x * TILE_SIZE.x 
            || hitbox.getPosition().y < 0 || hitbox.getPosition().y + hitbox.getSize().y > levelSize.y * TILE_SIZE.y) {
            if (axis == 'x') speed.x = 0;
            if (axis == 'y') {
                if (distance > 0) {
                    kill();
                } else {
                    speed.y = 0;
                }
            }
            return;
        } 

        // Check for collisions with level tiles before moving
        for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
            for (int y = playerGridPositonY - 1; y <= playerGridPositonY + 2; y++) {
                if (x < 0 || y < 0 || x >= levelSize.x || y >= levelSize.y) {
                    continue;
                }
                if (checkCollision(hitbox, tiles[x][y].getHitbox())) {
                    if (tiles[x][y].isSolid()) {
                        if (axis == 'x') speed.x = 0;
                        if (axis == 'y') speed.y = 0;
                        return;
                    }
                    if (tiles[x][y].isDangerous()) {
                        kill();
                        return;
                    }
                }
            }
        }

        sprite.move({axis == 'x' ? distance * deltaTime : 0, axis == 'y' ? distance * deltaTime : 0});
        moved -= abs(distance) * deltaTime;
    }

    
    return;
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
            if (x < 0 || y < 0 || x >= levelSize.x || y >= levelSize.y) {
                continue;
            }
            // Player is considered landing when he is about to hit the tile below (48 pixels margin)
            if (speed.y >= 0 && !groundedState && tiles[x][y].isSolid() && checkCollision(landingHitbox, tiles[x][y].getHitbox())) {
                resetAnimation();
                landingState = true;
            }
            if (tiles[x][y].isSolid() && checkCollision(feetHitbox, tiles[x][y].getHitbox())) {
                groundedState = true;
                jumpingState = false;
                canDash = true;
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

    cout << totalAnimationTimer << endl;
    
    if (totalAnimationTimer >= totalFrames * timePerFrame) {
        if (!repeat) {
            cout << "animation finished" << endl;
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
    cout << "friction" << endl;
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
    resetAnimation();
    jumpingState = true;
    speed.y = -375.0f;
}

void Player::faceLeft() {
    sprite.setOrigin({(float) frameWidth, 0});
    sprite.setScale({-1, 1});
    direction = -1;
}

void Player::faceRight() {
    sprite.setOrigin({0, 0});
    sprite.setScale({1, 1});
    direction = 1;
}

void Player::dash() {
    resetAnimation();
    dashingState = true;
    canDash = false; // Cannot dash again until the player touched the ground
    speed.x = direction * DASHING_SPEED;
}

void Player::kill() {
    resetAnimation();
    dyingState = true;
}