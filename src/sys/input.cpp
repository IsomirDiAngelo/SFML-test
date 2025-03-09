#include "input.h"

Input::Input() {}

void Input::updateKeyPress(Keyboard::Scancode keyPressed) {
    if (keysPressed.count(keyPressed) == 0) {
        keysPressed[keyPressed] = true;
    }
}

void Input::updateKeyRelease(Keyboard::Scancode keyReleased) {
    if (keysPressed.count(keyReleased) == 0) {
        keysPressed[keyReleased] = false;
    }
}

void Input::clear() {
    for (auto& key : keysPressed) {
        keysPressed.erase(key.first);
    }
}

bool Input::isKeyTriggered(Keyboard::Scancode keyScancode) {
    return (keysPressed.count(keyScancode) == 1 && keysPressed[keyScancode] == true);
}

bool Input::isKeyReleased(Keyboard::Scancode keyScancode) {
    return (keysPressed.count(keyScancode) == 1 && keysPressed[keyScancode] == false);
}