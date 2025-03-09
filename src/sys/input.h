#ifndef INPUT_H
#define INPUT_H

#include <SFML/Graphics.hpp> 
#include <iostream>

using namespace sf;
using namespace std;

class Input {
    private:
        unordered_map<Keyboard::Scancode, bool> keysPressed;

    public:
        Input();
        void updateKeyPress(Keyboard::Scancode keyPressed);
        void updateKeyRelease(Keyboard::Scancode keyReleased);
        void clear();
        bool isKeyTriggered(Keyboard::Scancode keyScancode);
        bool isKeyReleased(Keyboard::Scancode keyReleased);
};

#endif