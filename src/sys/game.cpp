#include "game.h"

Game::Game(Player& player, Camera& camera, Level& level) 
    : player(player), camera(camera), level(level), fpsDisplay(GAME_FONT) {
    pauseMenu = PauseMenu();
    fpsDisplay = Text(GAME_FONT);
}

void Game::run(float deltaTime, RenderWindow& window, Input& input) {
    if (input.isKeyTriggered(Keyboard::Scancode::Escape)) {
        pause = !pause;
        pauseMenu.resetCursor();
    }

    if (!pause && !gameFinished) {
        player.update(deltaTime, level, input);
        camera.update(player.getHitbox().getPosition(), level.getSize());
    }
    
    window.setView(camera.getView());
    window.draw(level);
    window.draw(player.getSprite());
    window.draw(player.getHitbox());

    if (!gameFinished) {
        for (int i = 0; i < level.entities.size(); i++) {
            window.draw(level.entities[i]->getSprite());
            level.entities[i]->update(deltaTime, player, window, gameFinished);
        }
    }

    if (DEBUG) {
        std::vector<std::vector<Tile>> tiles = level.getTiles();
        Vector2u levelSize = level.getSize();
        for (int i = 0; i < levelSize.x; i++) {
            for (int j = 0; j < levelSize.y; j++) {
                window.draw(tiles[i][j].getHitbox());
            }
        }
    }

    if (DEBUG || Keyboard::isKeyPressed(Keyboard::Key::F1)) {
        fpsDisplay.setString(to_string(1.0f / deltaTime));
        window.setView(window.getDefaultView());
        window.draw(fpsDisplay);
    }
    
    if (pause) {
        pauseMenu.update(deltaTime, pause, player, Vector2f(level.getSpawnPosition()), input, window);
    }
}

Player& Game::getPlayer() {
    return player;
}

void Game::setPlayer(Player& player) {
    this->player = player;
}

void Game::setLevel(Level& level) {
    this->level = level;
}