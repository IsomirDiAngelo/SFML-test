#ifndef PAUSE_MENU_H
#define PAUSE_MENU_H

#include "../entities/player.h"
#include "input.h"

class PauseMenu {
    private:
        RectangleShape menu;
        CircleShape circleCursor;
        Text continueButton;
        Text retryButton;
        Text quitButton;

        int pauseMenuIndex = -1;
        Vector2f pauseMenuCursorTimer = {0, 0};

        Vector2f levelSpawnPosition;

    public:
        PauseMenu();
        void update(float deltaTime, bool& pause, Player& player, 
            Vector2f levelSpawnPosition, Input& input, RenderWindow& window);
        void resetCursor();
};

#endif