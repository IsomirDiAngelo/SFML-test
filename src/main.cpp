#include <SFML/Graphics.hpp>
#include "entities/player.h"
#include "sys/tile.h"
#include "sys/level.h"
#include "util/globalConstants.h"
#include "sys/camera.h"
#include "sys/game.h"
#include "sys/input.h"
#include <iostream>

int main() {
    Clock clock;

    RenderWindow window(VideoMode(SCREEN_RESOLUTION), "SFML test project");
    window.setFramerateLimit(FRAMERATE_LIMIT);
    window.setKeyRepeatEnabled(false);
    
    Camera camera = Camera(SCREEN_RESOLUTION);
    Level level = Level(LEVEL_FILENAME, LEVEL_TILESET);
    Player player = Player(Vector2f(level.getSpawnPosition()));

    Game game = Game(player, camera, level);
    Input input = Input();

    while (window.isOpen()) {
        float deltaTime = clock.restart().asSeconds();
        Keyboard::Scancode keyPressed;
        Keyboard::Scancode keyReleased;

        while (const auto eventOpt = window.pollEvent()) {
            if (eventOpt->is<Event::Closed>()) {
                window.close();
            } else if (const auto* event = eventOpt->getIf<Event::KeyPressed>()) {
                keyPressed = event->scancode;
                input.updateKeyPress(keyPressed);
            } else if (const auto* event = eventOpt->getIf<Event::KeyReleased>()) {
                keyReleased = event->scancode;
                input.updateKeyRelease(keyReleased);
            }
        }
        
        window.clear();
    
        game.run(deltaTime, window, input);
        
        input.clear();

        window.display();
    }
}