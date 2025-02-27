#include "pauseMenu.h"

PauseMenu::PauseMenu() : continueButton(GAME_FONT), retryButton(GAME_FONT), quitButton(GAME_FONT) {
    menu = RectangleShape(Vector2f(SCREEN_RESOLUTION) - Vector2f({100, 100}));
    menu.setFillColor(Color(250, 150, 100, 100));
    menu.setPosition({50, 50});

    circleCursor = CircleShape(10.f);
    circleCursor.setOrigin(circleCursor.getLocalBounds().getCenter());

    continueButton = Text(GAME_FONT);
    continueButton.setString("CONTINUE");
    continueButton.setOrigin(continueButton.getLocalBounds().getCenter());
    continueButton.setPosition({SCREEN_RESOLUTION.x / 2, SCREEN_RESOLUTION.y / 2 - 50});

    retryButton = Text(GAME_FONT);
    retryButton.setString("RETRY");
    retryButton.setOrigin(retryButton.getLocalBounds().getCenter());
    retryButton.setPosition({SCREEN_RESOLUTION.x / 2, SCREEN_RESOLUTION.y / 2});

    quitButton = Text(GAME_FONT);
    quitButton.setString("QUIT");
    quitButton.setOrigin(quitButton.getLocalBounds().getCenter());
    quitButton.setPosition({SCREEN_RESOLUTION.x / 2, SCREEN_RESOLUTION.y / 2 + 50});
}

void PauseMenu::update(float deltaTime, bool& pause, Player& player, 
        Vector2f levelSpawnPosition, Input& input, RenderWindow& window) {

    if (input.isKeyTriggered(Keyboard::Scancode::Space) || input.isKeyTriggered(Keyboard::Scancode::Enter)) {
        bool deathAnimationFinished = false;
        switch (pauseMenuIndex) {
            case 1:
                window.close();
                break;
            case 0:
                player.kill();
                pause = false;
                break;
            default:
                pause = false;
        }
    }

    if (input.isKeyTriggered(Keyboard::Scancode::Down)) { 
        if (pauseMenuIndex < 1) {
            pauseMenuIndex = pauseMenuIndex + 1;
        }
    }
    
    if (input.isKeyTriggered(Keyboard::Scancode::Up)) {
        if (pauseMenuIndex > -1) {
            pauseMenuIndex--;
        }
    }
    
    circleCursor.setPosition({70, SCREEN_RESOLUTION.y / 2 + pauseMenuIndex * 50});

    window.setView(window.getDefaultView());
    window.draw(menu);
    window.draw(continueButton);
    window.draw(retryButton);
    window.draw(quitButton);
    window.draw(circleCursor);
}

void PauseMenu::resetCursor() {
    pauseMenuIndex = -1;
}