#include "mapEntity.h"

MapEntity::MapEntity(MapEntityType type, Vector2f spawnPosition) : sprite(texture), tutorialTextBox(GAME_FONT) {
    this->type = type;
    if (type == MapEntityType::TUTORIAL_ARROW) {
        texture = Texture(TUTORIAL_ARROW_FILENAME, false, IntRect({0, 0}, Vector2i(TILE_SIZE)));
        hitbox = RectangleShape(Vector2f({16, 48}));
    } else {
        texture = Texture(SACRED_FRUIT_FILENAME, false, IntRect({0, 0}, Vector2i(TILE_SIZE)));
        hitbox = RectangleShape(Vector2f(TILE_SIZE));
    }
    // texture = Texture(textureFilename, false, IntRect({0, 0}, Vector2i(TILE_SIZE)));
    sprite = Sprite(texture);

    sprite.setPosition(spawnPosition);
    hitbox.setPosition(spawnPosition);
    hitbox.setFillColor(Color::Transparent);
    hitbox.setOutlineThickness(-1);
    hitbox.setOutlineColor(Color::Red);

    tutorialTextBox = Text(GAME_FONT);

    animationTimer = 0.0f;
    up = false;
}

MapEntity::MapEntity(MapEntityType type, Vector2f spawnPosition, string tutorialString) 
    : MapEntity(type, spawnPosition) {
    tutorialTextBox.setString(tutorialString);
    tutorialTextBox.setCharacterSize(20);
    tutorialTextBox.setOutlineThickness(1);
    tutorialTextBox.setOutlineColor(Color::Black);
    tutorialTextBox.setOrigin(tutorialTextBox.getGlobalBounds().getCenter());
    tutorialTextBox.setPosition(spawnPosition - Vector2f(0, 32));
}

MapEntity::~MapEntity() {
    cout << "Map entity destroyed" << endl;
}

void MapEntity::update(float deltaTime, Player& player, RenderWindow& window, bool& gameFinished) {
    animate(deltaTime);
    if (player.checkCollision(hitbox, player.getHitbox())) {
        switch (type) {
            case MapEntityType::TUTORIAL_ARROW:
                window.draw(tutorialTextBox);
                break;
            case MapEntityType::SACRED_FRUIT:
                gameFinished = true;
                break;
            default:
                break;
        }
    }
}

void MapEntity::animate(float deltaTime) {
    animationTimer += deltaTime;
    if (animationTimer > 0.3f) {
        animationTimer = 0.0f;
        sprite.move(Vector2f(0, up ? 1 : -1));
        up = !up;
    }
}

MapEntityType MapEntity::getType() {
    return type;
}

Sprite& MapEntity::getSprite() {
    return sprite;
}

Text& MapEntity::getText() {
    return tutorialTextBox;
}

RectangleShape& MapEntity::getHitbox() {
    return hitbox;
}