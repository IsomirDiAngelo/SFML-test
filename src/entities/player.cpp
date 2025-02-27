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
}

RectangleShape& Player::getHitbox() {
    return hitbox;
}

Sprite& Player::getSprite() {
    return sprite;
}

void Player::update(float deltaTime, Level& level, Input& input) {
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
        sprite.setOrigin({frameWidth, 0});
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
    if (input.isKeyTriggered(Keyboard::Scancode::Space) && groundedState) {
        speed.y = -375.0f;
    }

    // Falling
    if (!groundedState) {
        // Increase gravity upon falling
        float accelerationMultiplier = speed.y < 0 ? 1.0f : 1.5f;
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

    if (abs(speed.x) > 0 && abs(speed.x) <= MAX_SPEED_WALKING + 25.0f) {
        animate(deltaTime, SPRITE_OFFSET_WALKING, WALKING_FRAMES);
    } else if (abs(speed.x) > MAX_SPEED_WALKING + 25.0f) {
        animate(deltaTime, SPRITE_OFFSET_RUNNING, RUNNING_FRAMES);
    } else if (abs(speed.x) == 0) {
        currentFrame = 0;
        sprite.setTextureRect(IntRect({0, 0}, {frameWidth, frameHeight}));
    }

    updatePosition(speed.x * deltaTime, speed.y * deltaTime, level);
}

/**
 * Update player position v2
 * Handle movements and collision (AABB) pixel by pixel
 * 
 * More robust than v1
 */
void Player::updatePosition(float dx, float dy, Level& level) {
    Vector2f remainder = {};

    std::vector<std::vector<Tile>> tiles = level.getTiles();
    Vector2u levelSize = level.getSize();

    // Following instructions to be refractored to avoid code duplication

    remainder.y += dy;
    int moveY = round(remainder.y);

    if (moveY != 0) {
        remainder.y -= moveY;
        int moveSign = moveY > 0 ? 1 : -1;

        auto movePlayerY = [&] {
            while (moveY != 0) {
                int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
                int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;
                
                hitbox.setPosition(hitbox.getPosition() + Vector2f(0, moveSign));

                for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
                    for (int y = playerGridPositonY - 1; y <= playerGridPositonY + 2; y++) {
                        if (x < 0 || y < 0 || x >= levelSize.x || y >= levelSize.y) {
                            continue;
                        }
                        if (tiles[x][y].isSolid() && checkCollision(hitbox, tiles[x][y].getHitbox())) {
                            speed.y = 0;
                            return;
                        }
                    }
                }
                sprite.move({0, moveSign});
                moveY -= moveSign;
            }
        };
        movePlayerY();
    }

    updateHitbox();

    remainder.x += dx;
    int moveX = round(remainder.x);

    if (moveX != 0) {
        remainder.x -= moveX;
        int moveSign = moveX > 0 ? 1 : -1;

        auto movePlayerX = [&] {
            while (moveX != 0) {
                int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
                int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;

                hitbox.setPosition(hitbox.getPosition() + Vector2f(moveSign, 0));

                for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
                    for (int y = playerGridPositonY - 1; y <= playerGridPositonY + 2; y++) {
                        if (x < 0 || y < 0 || x >= levelSize.x || y >= levelSize.y) {
                            continue;
                        }
                        if (tiles[x][y].isSolid() == true && checkCollision(hitbox, tiles[x][y].getHitbox()) == true) {
                            speed.x = 0;
                            return;
                        }
                    }
                }
                sprite.move({moveSign, 0});
                moveX -= moveSign;
            }
        };
        movePlayerX();
    }

    updateHitbox();
    updateGroundedState(tiles, levelSize);
}

/**
 * Update player grounded state by checking if no tile below us is solid and colliding with the player's feet
 */
void Player::updateGroundedState(std::vector<std::vector<Tile>>& tiles, Vector2u levelSize) {
    int playerGridPositonX = hitbox.getPosition().x / TILE_SIZE.x;
    int playerGridPositonY = hitbox.getPosition().y / TILE_SIZE.y;
    int y = playerGridPositonY + 2; // The range of tiles below the player is at Y + 2 because the player is 2 tiles tall

    RectangleShape feetHitbox = RectangleShape(hitbox.getSize());
    feetHitbox.setPosition(hitbox.getPosition() + Vector2f({0, 1}));

    for (int x = playerGridPositonX - 1; x <= playerGridPositonX + 1; x++) {
        if (x < 0 || y < 0 || x >= levelSize.x || y >= levelSize.y) {
            continue;
        }
        if (tiles[x][y].isSolid() && checkCollision(feetHitbox, tiles[x][y].getHitbox())) {
            groundedState = true;
            return;
        }
    }

    // Keep in memory the last value of speed.x before jumping or falling off a ledge
    airboneXSpeedSnapshot = speed.x;
    groundedState = false;
}

/**
 * Update player position v1
 * Handle movements and collision (AABB) using speed and a cross shaped player hitbox to precisely reposition
 * 
 * Precise but not very robust, does not work well with some edge cases (hitting a corner from below with high velocity)
 */
// void updatePosition(float x, float y, std::vector<std::vector<Tile>>& tiles, RenderWindow& window) {
//     Vector2i remainder = {round(x), round(y)};

//     RectangleShape newHitbox = RectangleShape({12, 28});
//     newHitbox.setPosition(hitbox.getPosition() + Vector2f(x, y));
//     newHitbox.setFillColor(Color::Transparent);
    
//     RectangleShape newArmsHitbox = RectangleShape({12, 14});
//     newArmsHitbox.setPosition(armsHitbox.getPosition() + Vector2f(x, 0));
//     newArmsHitbox.setFillColor(Color::Transparent);

//     RectangleShape newFeetAndHeadHitbox = RectangleShape({5, 28});
//     newFeetAndHeadHitbox.setPosition(feetAndHeadHitbox.getPosition() + Vector2f(0, y));
//     newFeetAndHeadHitbox.setFillColor(Color::Transparent);
    

//     bool collisionX = false;
//     bool collisionY = false;
//     for (auto& row : tiles) {
//         for (auto& tile : row) {
//             if (tile.isSolid()) {
//                 if (checkCollision(newFeetAndHeadHitbox, tile.getHitbox())) {
//                     float distanceY1 = newFeetAndHeadHitbox.getPosition().y - (tile.getHitbox().getPosition().y + 16); // distance from top of player to the bottom of the tile
//                     float distanceY2 = newFeetAndHeadHitbox.getPosition().y + 28 - tile.getHitbox().getPosition().y; // distance from bottom of player to the top of the tile

//                     std::cout << distanceY1 << "," << distanceY2 << " on tile " << tile.getHitbox().getPosition().x << "," << tile.getHitbox().getPosition().y << std::endl;
                    
//                     if (abs(distanceY2) > abs(distanceY1)) {
//                         sprite.move({0, y + abs(distanceY1)});
//                         std::cout << "Correcting +Y" << std::endl;
//                     } else {
//                         sprite.move({0, y - abs(distanceY2)});
//                         std::cout << "Correcting -Y" << std::endl;
//                     }
//                     speed.y = 0;
//                     collisionY = true;
//                     updateHitboxes();
//                     newArmsHitbox.setPosition(armsHitbox.getPosition() + Vector2f(x, 0));
//                     newFeetAndHeadHitbox.setPosition(feetAndHeadHitbox.getPosition());
//                 } 
//                 if (checkCollision(newArmsHitbox, tile.getHitbox())) {
//                     float distanceX1 = newArmsHitbox.getPosition().x - (tile.getHitbox().getPosition().x + 16);
//                     float distanceX2 = newArmsHitbox.getPosition().x + 12 - tile.getHitbox().getPosition().x;
                    
//                     if (abs(distanceX2) > abs(distanceX1)) {
//                         sprite.move({x + abs(distanceX1), 0});
//                         std::cout << "Correcting -X" << std::endl;
//                     } else {
//                         sprite.move({x - abs(distanceX2), 0});
//                         std::cout << "Correcting +X" << std::endl;
//                     }
//                     speed.x = 0;
//                     collisionX = true;
//                     updateHitboxes();
//                     newArmsHitbox.setPosition(armsHitbox.getPosition());
//                     newFeetAndHeadHitbox.setPosition(feetAndHeadHitbox.getPosition() + Vector2f(0, y));
                    
//                 }
//             }
//         }
//     }

//     sprite.move({collisionX ? 0 : x,  collisionY ? 0 : y});
//     window.draw(newHitbox);
//     updateHitboxes();
// }

void Player::updateHitbox() {
    hitbox.setPosition({sprite.getPosition().x + HITBOX_OFFSET.x, sprite.getPosition().y + HITBOX_OFFSET.y});
}

void Player::animate(float deltaTime, float offset, int totalFrames) {
    animationTimer += deltaTime;
    if (animationTimer > 0.1f) {
        currentFrame = (currentFrame + 1) % totalFrames;
        sprite.setTextureRect(IntRect({currentFrame * frameWidth, offset}, {frameWidth, frameHeight}));
        animationTimer = 0.0f;
    }
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
