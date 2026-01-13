#include "enemySpawner.h"

Asteroid::Asteroid(float _x, float _y, float _w, float _h, SDL_Color _color, int _z) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    color = _color;
    zIndex = _z;
    speed = 100.0f; // px per second
    hp = 5; 
    maxHp = 5;
    maxCooldown = 0.5f;
    attackPower = 2;
    rect = {(int)x, (int)y, (int)w, (int)h};
}

void Asteroid::update(float dt) {
    applyMovement(x, y, dt);
    if (damageCooldown > maxCooldown - flashWindow) {
        color = {255, 0, 0, 255};
    } else {
        color = {200, 200, 200, 255};
    }
    rect = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
}

EnemySpawner::EnemySpawner() {
}
EnemySpawner::~EnemySpawner() {
}
