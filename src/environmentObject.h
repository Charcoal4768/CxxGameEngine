#pragma once
#include "basicEntity.h"

enum class EnvType {
    Platform,   // Solid ground
    Decoration, // Non-collidable (trees, posters)
    Lava,       // Damage-dealing zones
    Ladder,     // Vertical movement zones
    Water       // Physics-altering zones
};

struct EnvironmentObject : public gameObject {
    EnvType type;
    int zIndex; // Useful for your RenderPipeline!

    // Constructor for easy creation
    EnvironmentObject(float _x, float _y, float _w, float _h, SDL_Color _color, EnvType _type, int _z);
};