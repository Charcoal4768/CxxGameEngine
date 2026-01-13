#include "environmentObject.h"

EnvironmentObject::EnvironmentObject(float _x, float _y, float _w, float _h, SDL_Color _color, EnvType _type, int _z) {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    color = _color;
    type = _type;
    zIndex = _z;
    
    // Initialize the SDL_Rect immediately
    rect = { static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h) };
}

bool isSolid(EnvType type) {
    return (type == EnvType::Platform);
}

bool isHazardous(EnvType type) {
    return (type == EnvType::Lava);
}

bool isClimbable(EnvType type) {
    return (type == EnvType::Ladder);
}
