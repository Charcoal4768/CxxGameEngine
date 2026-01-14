#include "enemySpawner.h"
#include <cstdio>
#include <cmath>

Asteroid::Asteroid(float _x, float _y, float _w, float _h, float _velX, float _velY, SDL_Color _color, int _z, SDL_Texture *tex)
{
    x = _x;
    y = _y;
    w = _w;
    h = _h;
    velX = _velX;
    velY = _velY;
    color = _color;
    zIndex = _z;
    speed = 150.0f;
    hp = 5;
    maxHp = 5;
    maxCooldown = 1.0f;
    color = _color;
    baseColor = color;
    attackPower = 4;
    fakeFriction = 0.2f;
    rect = {(int)x, (int)y, (int)w, (int)h};
    texture = tex;
    // team = 1; // Enemy team
    teamID = 1;
}

void Asteroid::update(float dt)
{
    if (isDed) {
        active = false;
        return;
    }
    applyMovement(x, y, dt);
    flashRedOnDamage();
    rect = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
    // printf("Damage Cooldown: %.2f seconds\n", damageCooldown);

}

void Asteroid::takeDamage(Damaging* source)
{
    if (source->teamID == teamID) return; // Ignore friendly fire
    Damageable::takeDamage(source);
    printf("Asteroid took %d damage! HP left: %d\n", source->attackPower, hp);
}

void Asteroid::flashRedOnDamage()
{
    if (damageCooldown > maxCooldown - flashWindow)
    {
        color = {255, 0, 0, 255};
    }
    else
    {
        color = baseColor;
    }
}

void Asteroid::respawn(Point2D spawnPoint, float velX, float velY)
{
    x = static_cast<float>(spawnPoint.x);
    y = static_cast<float>(spawnPoint.y);
    this->velX = velX;
    this->velY = velY;
    hp = maxHp;
    isDed = false;
    active = true;
}

EnemySpawner::EnemySpawner(SDL_Window *win, std::vector<gameObject *> _scene, int marginY, int marginX, int maxAlive, int batchSize, int coolDown, bool edgesOnly)
{
    int height, width;
    this->edgesOnly = edgesOnly;
    SDL_GetWindowSize(win, &width, &height);
    this->scene = &_scene; // save a pointer to the scene objects to update later
    // possible positions: width-marginX to width, 0 to marginX, 0 to marginY, height-marginY to height - edgesOnly
    // possible positions: marginX to width-marginX, marginY to height-marginY - !edgesOnly
    // generate a table of randomized velocities for spawning enemies, max 10 different ones
    // apply said velocities to enemies as they spawn from the spawn points
    if (edgesOnly)
    {
        // set randomized batchSize spawnpoints for edges only
        for (int i = 0; i < batchSize; i++)
        {
            int edge = rand() % 4; // 0: top, 1: bottom, 2: left, 3: right
            Point2D spawnPoint;
            // Velocity2D randomizedVelocity;
            // randomizedVelocity.velX = static_cast<double>(rand() % 100 - 50);
            // randomizedVelocity.velY = static_cast<double>(rand() % 100 - 50);
            switch (edge)
            {
            case 0: // top
                spawnPoint.x = static_cast<double>(rand() % (width - 2 * marginX) + marginX);
                spawnPoint.y = static_cast<double>(rand() % marginY);
                break;
            case 1: // bottom
                spawnPoint.x = static_cast<double>(rand() % (width - 2 * marginX) + marginX);
                spawnPoint.y = static_cast<double>(height - (rand() % marginY));
                break;
            case 2: // left
                spawnPoint.x = static_cast<double>(rand() % marginX);
                spawnPoint.y = static_cast<double>(rand() % (height - 2 * marginY) + marginY);
                break;
            case 3: // right
                spawnPoint.x = static_cast<double>(width - (rand() % marginX));
                spawnPoint.y = static_cast<double>(rand() % (height - 2 * marginY) + marginY);
                break;
            }
            spawnPoints.emplace_back(spawnPoint);
            // velocityTable.emplace_back(randomizedVelocity);
        }
    }
    else
    {
        for (int i = 0; i < batchSize; i++)
        {
            Point2D spawnPoint;
            spawnPoint.x = static_cast<double>(rand() % (width - 2 * marginX) + marginX);
            spawnPoint.y = static_cast<double>(rand() % (height - 2 * marginY) + marginY);
            spawnPoints.emplace_back(spawnPoint);
        }
    }
}

std::vector<gameObject *> *EnemySpawner::StartSpawning()
{
    paused = false;
    // Logic to start spawning enemies would go here
    // Just debug for now
    printf("Enemy Spawner Started\n");
    printf("Spawn Points:\n");
    for (const auto &point : spawnPoints)
    {
        // float velX = velocityTable[rand() % velocityTable.size()].velX;
        // float velY = velocityTable[rand() % velocityTable.size()].velY;
        // if edgesOnly is true, make sure velocities point inward
        float velX = static_cast<float>(rand() % 200 - 100);
        float velY = static_cast<float>(rand() % 200 - 100);
        if (edgesOnly)
        {
            if (point.x < 100) // left edge
            {
                velX = fabs(velX);
            }
            else if (point.x > 700) // right edge
            {
                velX = -fabs(velX);
            }
            if (point.y < 100) // top edge
            {
                velY = fabs(velY);
            }
            else if (point.y > 500) // bottom edge
            {
                velY = -fabs(velY);
            }
        }
        scene->emplace_back(new Asteroid(point.x, point.y, 100, 100, velX, velY, {200, 200, 200, 255}, 1, nullptr));
    }
    return scene;
}

EnemySpawner::~EnemySpawner()
{
    for (auto enemy : *scene)
    {
        delete enemy;
    }
}
