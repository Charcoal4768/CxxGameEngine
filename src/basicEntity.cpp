#include "basicEntity.h"
#include <cmath>

void Moveable::applyMovement(float &x, float &y, float dt)
{
    velX += accX * dt;
    velY += accY * dt;

    if (!falling)
    {
        float dragAmount = fakeFriction * dt;
        if (velX > 0)
        {
            velX -= dragAmount;
            if (velX < 0) velX = 0;
        }
        else if (velX < 0)
        {
            velX += dragAmount;
            if (velX > 0) velX = 0;
        }
    }
    x += velX * dt;
    y += velY * dt;

}

void Moveable::jump(float force)
{
    if (!falling)
    {
        velY = -force;
        falling = true;
    }
}

void Damageable::takeDamage(Damaging* source)
{
    // If cooldown is active, ignore damage
    if (damageCooldown > 0) return;

    hp -= source->attackPower;
    
    // Start the cooldown timer
    damageCooldown = maxCooldown;

    if (hp <= 0)
    {
        hp = 0;
        isDed = true;
    }
}

void gameObject::submitRenderRequest(RenderPipeline &pipeline, int overrideZ)
{
    if (!active) return;

    DrawRequest req;
    req.rect = this->rect;
    req.color = this->color;
    req.zIndex = (overrideZ != -1) ? overrideZ : this->zIndex;
    
    // Pass the new data
    req.texture = this->texture; 
    req.angle = this->angle;
    req.flip = this->flip;

    pipeline.submit(req);
}


void gameObject::update(float dt)
{
    // Default behavior: keep SDL_Rect in sync with float position/size
    rect = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
}

void CollisionGrid::insert(gameObject* obj){
    // Calculate the range of cells this object covers
    int startX = static_cast<int>(obj->x) / cellSize;
    int endX   = static_cast<int>(obj->x + obj->w) / cellSize; // Right edge
    int startY = static_cast<int>(obj->y) / cellSize;
    int endY   = static_cast<int>(obj->y + obj->h) / cellSize; // Bottom edge

    // Add the object to every cell it touches
    for (int x = startX; x <= endX; x++) {
        for (int y = startY; y <= endY; y++) {
            grid[{x, y}].push_back(obj);
        }
    }
}

void CollisionGrid::clear(){grid.clear();}

std::vector<gameObject*> CollisionGrid::getNearby(gameObject* obj){
    std::vector<gameObject *> nearby;
    int cellX = static_cast<int>(obj->x) / cellSize;
    int cellY = static_cast<int>(obj->y) / cellSize;

    for (int i = -1; i <= 1; ++i)
    {
        for (int j = -1; j <= 1; ++j)
        {
            auto it = grid.find({cellX + i, cellY + i});
            if (it != grid.end())
            {
                nearby.insert(nearby.end(), it->second.begin(), it->second.end());
            }
        }
    }

    return nearby;
}

