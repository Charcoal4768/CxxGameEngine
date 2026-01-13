#include "advancedObjects.h"

Wall::Wall(float _x, float _y, float _width, float _height, SDL_Color _color, int _z)
{
    x = _x;
    y = _y;
    w = _width;
    h = _height;
    color = _color;
    zIndex = _z;
    collidable = true;

    rect = {(int)x, (int)y, (int)w, (int)h};
}

Lava::Lava(float _x, float _y, float _w, float _h, int _z)
    : EnvironmentObject(_x, _y, _w, _h, {255, 100, 0, 255}, EnvType::Lava, _z)
{
    attackPower = 2; // Damage dealt per frame/contact
    collidable = false;
}
