#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

class Player;

#include "../util/globalConstants.h"
#include "player.h"

#define TUTORIAL_ARROW_FILENAME "assets/entities/tutorial arrow.png"
#define SACRED_FRUIT_FILENAME "assets/entities/sacred fruit.png"

enum class MapEntityType { _NULL, TUTORIAL_ARROW, SACRED_FRUIT };

// I should have written an entity super class to prevent code duplication with Player class and two subclasses for tutorial arrow and sacred fruit

class MapEntity {

    private:
        MapEntityType type;
        Texture texture;
        RectangleShape hitbox;
        Sprite sprite;

        Text tutorialTextBox;
        bool sacredFruitCollected;

        float animationTimer;
        bool up;

    public:
        MapEntity(MapEntityType type, Vector2f spawnPosition);
        MapEntity(MapEntityType type, Vector2f spawnPosition, string tutorialText);
        ~MapEntity();
        void update(float deltaTime, Player& player, RenderWindow& window, bool& gameFinished);
        void animate(float deltaTime);
        Sprite& getSprite();
        RectangleShape& getHitbox();
        Text& getText();
        MapEntityType getType();

};

#endif