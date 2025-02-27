#include "game.h"

Game::Game(Player& player, Camera& camera, Level& level) 
    : player(player), camera(camera), level(level) {
    pauseMenu = PauseMenu();
}

void Game::run(float deltaTime, RenderWindow& window, Input& input) {
    if (input.isKeyTriggered(Keyboard::Scancode::Escape)) {
        pause = !pause;
    }

    if (!pause) {
        player.update(deltaTime, level, input);
        camera.update(player.getHitbox().getPosition(), level.getSize());
    }
    
    window.setView(camera.getView());
    window.draw(level);
    window.draw(player.getSprite());
    window.draw(player.getHitbox());

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