#include "camera.h" 
#include "../util/globalConstants.h"

Camera::Camera() {}

Camera::Camera(Vector2u size) {
    this->view = View(FloatRect({0.0f, 0.0f}, {(float) size.x, (float) size.y}));
    this->offset = {0.0f, 0.0f};
}

void Camera::update(Vector2f playerPosition, Vector2u levelSize) {
    Vector2f newCameraPosition = playerPosition + Vector2f({0, 0});
    if (newCameraPosition.x < SCREEN_RESOLUTION.x / 2) {
        newCameraPosition.x = SCREEN_RESOLUTION.x / 2;
    } else if (newCameraPosition.x > levelSize.x * TILE_SIZE.x - SCREEN_RESOLUTION.x / 2) {
        newCameraPosition.x = levelSize.x * TILE_SIZE.x - SCREEN_RESOLUTION.x / 2;
    } 
    if (newCameraPosition.y < SCREEN_RESOLUTION.y / 2) {
        newCameraPosition.y = SCREEN_RESOLUTION.y / 2;
    } else if (newCameraPosition.y > levelSize.y * TILE_SIZE.y - SCREEN_RESOLUTION.y / 2) {
        newCameraPosition.y = levelSize.y * TILE_SIZE.y - SCREEN_RESOLUTION.y / 2;
    }
    this->view.setCenter(newCameraPosition);
    this->offset = newCameraPosition - Vector2f({SCREEN_RESOLUTION.x / 2, SCREEN_RESOLUTION.y / 2});
}

View& Camera::getView() {
    return view;
}