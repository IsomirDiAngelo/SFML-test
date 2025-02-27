#ifndef GAME_H
#define GAME_H

#include "../entities/player.h"
#include "camera.h"
#include "level.h"
#include "pauseMenu.h"
#include "input.h"

#define LEVEL_FILENAME "assets/levels/test.lvl"
#define LEVEL_TILESET "assets/tiles/tiles.png"

class Game {
    private:
        Player player;
        Camera camera;
        Level level;

        PauseMenu pauseMenu;

        bool pause = false;      

    public:
        Game(Player& player, Camera& camera, Level& level);
        void run(float deltaTime, RenderWindow& window, Input& input);
        Player& getPlayer();
        void setPlayer(Player& player);
        void setLevel(Level& level);
};

#endif