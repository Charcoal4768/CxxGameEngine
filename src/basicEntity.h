#pragma once
#include <SDL.h>
#include <vector>
#include "renderPipeline.h"
#include <map>

struct gameObject
{
    float x,y,w,h;
    SDL_Rect rect;
    SDL_Color color;
    SDL_Texture* texture = nullptr;
    SDL_Point* center = nullptr;
    SDL_RendererFlip flip = SDL_FLIP_NONE;
    double angle = 0.0;
    int zIndex = 0;
    bool active = true;
    bool collidable = true;

    virtual void submitRenderRequest(RenderPipeline& pipeline, int overrideZ = -1);
    // Update called each frame with delta-time (seconds). Default keeps rect in sync.
    virtual void update(float dt);
    virtual ~gameObject() = default;
};

struct Point2D{
    double x = 0.0;
    double y = 0.0;
};

struct Velocity2D{
    double velX = 0.0;
    double velY = 0.0;
};

struct Moveable{
    float velX = 0, velY = 0;
    float accX = 0, accY = 0;

    float fakeFriction = 0.1f;
    float gravity = 980.0f;
    float speed = 0;
    float bouncy = 0.5f;

    bool falling = true;

    // applyMovement should consider delta-time when moving
    virtual void applyMovement(float& x, float& y, float dt);
    void jump(float force);
    virtual ~Moveable() = default;
};

struct Damaging{
    int attackPower = 1;
    float knockBack = 0.5f;

    int teamID = 0;
    virtual ~Damaging() = default;
};

struct Damageable {
    int maxHp = 10;
    int hp = 10;
    bool isDed = false;

    // Cooldown logic
    float damageCooldown = 0.0f; 
    float maxCooldown = 0.5f; // Half a second of invulnerability

    virtual void takeDamage(Damaging* source);
    virtual ~Damageable() = default;
};

class CollisionGrid{
private:
    /* data */
public:
    int cellSize = 1025;

    std::map<std::pair<int,int>, std::vector<gameObject*>> grid;

    void clear();
    void insert(gameObject* obj);
    std::vector<gameObject*> getNearby(gameObject* obj);
};
