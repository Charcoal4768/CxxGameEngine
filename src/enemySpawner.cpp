#include "enemySpawner.h"
#include <cstdio>
#include <cmath>
#include <SDL2/SDL_image.h>
#include <iostream>

// Generate random asteroid size keeping width and height within 10 units of each other
std::pair<float, float> GenerateAsteroidSize(int minSize = 40, int maxSize = 200)
{
    float baseSize = static_cast<float>(rand() % (maxSize - minSize) + minSize);
    float sizeVariation = static_cast<float>(rand() % 21 - 10); // -10 to +10
    return {baseSize, baseSize + sizeVariation};
}

Asteroid::Asteroid(float _x, float _y, float _w, float _h, float _velX, float _velY, SDL_Color _color, int _z, int _xRange, int _yRange, SDL_Texture *tex, SDL_Renderer *renderer)
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
    fakeFriction = 0.02f;
    rect = {(int)x, (int)y, (int)w, (int)h};
    if (tex && renderer) texture = tex;
    // team = 1; // Enemy team
    teamID = 1;
    xRange = _xRange;
    yRange = _yRange;
}

void Asteroid::update(float dt)
{
    if (isDed) {
        active = false;
        return;
    }
    if (!active) return;
    applyMovement(x, y, dt);
    if (x+w<0) active = false; // outside left bounds
    if (x> xRange) active = false; // outside right bounds
    if (y+h<0) active = false; // outside top bounds
    if (y> yRange) active = false; // outside bottom bounds
    if (velX < 0.1f && velY < 0.1f) active = false; // almost stopped 
    angle += 20.0f * dt;
    if (angle >= 360.0f) angle -= 360.0f;
    flashRedOnDamage();
    rect = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
    // printf("Damage Cooldown: %.2f seconds\n", damageCooldown);

}

void Asteroid::takeDamage(Damaging* source)
{
    if (!active) return;
    if (source->teamID == teamID) return; // Ignore friendly fire
    Damageable::takeDamage(source);
}

void Asteroid::flashRedOnDamage()
{
    if (!active) return;
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

EnemySpawner::EnemySpawner(SDL_Window *win, SDL_Renderer *renderer, std::vector<gameObject *> _scene, int marginY, int marginX, int maxAlive, int batchSize, int coolDown, bool edgesOnly)
{
    int height, width;
    this->edgesOnly = edgesOnly;
    SDL_GetWindowSize(win, &width, &height);
    windowSize = {width, height};
    maxAliveEnemies = maxAlive;
    this->batchSize = batchSize;
    maxCoolDown = static_cast<float>(coolDown);
    this->scene = &_scene; // save a pointer to the scene objects to update later
    this->renderer = renderer;
    // possible positions: width-marginX to width, 0 to marginX, 0 to marginY, height-marginY to height - edgesOnly
    // possible positions: marginX to width-marginX, marginY to height-marginY - !edgesOnly
    // generate a table of randomized velocities for spawning enemies, max 10 different ones
    // apply said velocities to enemies as they spawn from the spawn points
    if (edgesOnly)
    {
        for (int i = 0; i < maxAlive; i++)
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
        for (int i = 0; i < maxAlive; i++)
        {
            Point2D spawnPoint;
            spawnPoint.x = static_cast<double>(rand() % (width - 2 * marginX) + marginX);
            spawnPoint.y = static_cast<double>(rand() % (height - 2 * marginY) + marginY);
            spawnPoints.emplace_back(spawnPoint);
        }
    }
}

std::vector<gameObject *> *EnemySpawner::InitializeEnemies()
{
    paused = true;
    int spawnedCount = 0;
    this->scene->clear();
    this->coolDownTimer = 0.0f;
    std::string texturePath = "assets/textures/asteroid.png";
    SDL_Texture *texture = IMG_LoadTexture(renderer, texturePath.c_str());
    // Just debug for now
    printf("Enemy Spawner Started\n");
    printf("Spawn Points:\n");
    for (const auto &point : spawnPoints)
    {
        // float velX = velocityTable[rand() % velocityTable.size()].velX;
        // float velY = velocityTable[rand() % velocityTable.size()].velY;
        // if edgesOnly is true, make sure velocities point inward
        // initialize maxAlive enemies at random spawn points
        if (spawnedCount >= maxAliveEnemies) break;
        // Direct toward screen center with randomized speed and angle noise
        float centerX = windowSize.first / 2.0f;
        float centerY = windowSize.second / 2.0f;
        float angle = atan2(centerY - point.y, centerX - point.x);
        float angleNoise = (rand() / (float)RAND_MAX - 0.5f) * 0.4f; // ±0.2 radians
        float baseSpeed = 50.0f + (rand() % 100); // 50-150
        float velX = baseSpeed * cos(angle + angleNoise);
        float velY = baseSpeed * sin(angle + angleNoise);
        auto [width, height] = GenerateAsteroidSize();
        scene->emplace_back(new Asteroid(point.x, point.y, width, height, velX, velY, {200, 200, 200, 255}, 1, windowSize.first, windowSize.second, texture, renderer));
        // make the asteroid not active initially
        scene->back()->active = false;
        spawnedCount++;
    }
    return scene;
}

void EnemySpawner::StartSpawning()
{
    paused = false;
    printf("Enemy Spawner Started\n");
}

void EnemySpawner::StopSpawning()
{
    // Logic to stop spawning enemies would go here
    paused = true;
    printf("Enemy Spawner Stopped\n");
}

void EnemySpawner::SpawnBatch()
{
    //reactivate batchSize inactive enemies ideally but only if maxAlive - currentlyAlive > batchSize, else spawn only the difference
    if(currentlyAlive >= maxAliveEnemies) return;
    int toSpawn;
    if (currentlyAlive + batchSize > maxAliveEnemies)
    {
        toSpawn = maxAliveEnemies - currentlyAlive;
    }
    else
    {
        toSpawn = batchSize;
    }
    int spawned = 0;
    for (auto obj : *scene)
    {
        if (obj->active) continue;
        if (spawned >= toSpawn) break;
        Asteroid* asteroid = dynamic_cast<Asteroid*>(obj);
        if (asteroid) {
            Point2D spawnPoint = spawnPoints[rand() % spawnPoints.size()];
            // Direct toward screen center with randomized speed and angle noise
            float centerX = windowSize.first / 2.0f;
            float centerY = windowSize.second / 2.0f;
            float angle = atan2(centerY - spawnPoint.y, centerX - spawnPoint.x);
            float angleNoise = (rand() / (float)RAND_MAX - 0.5f) * 0.4f; // ±0.2 radians
            float baseSpeed = 50.0f + (rand() % 100); // 50-150
            float velX = baseSpeed * cos(angle + angleNoise);
            float velY = baseSpeed * sin(angle + angleNoise);
            asteroid->respawn(spawnPoint, velX, velY);
        }
        spawned++;
    }
    if (spawned > 0)
    {
        coolDownTimer = maxCoolDown;
        printf("Spawned %d enemies. Currently alive: %d\n", spawned, currentlyAlive);
    }
}

void EnemySpawner::Update(float dt)
{
    // Update all the cooldowns and timers
    // spawn batchSize enemies every maxCoolDown seconds until maxAliveEnemies is reached
    // spawn them by reactivating inactive enemies from the scene
    // place them at random spawn points w/ random velocities
    if (paused) return;
    coolDownTimer -= dt;
    if (coolDownTimer < 0) coolDownTimer = 0;
    if (coolDownTimer > 0) return;
    int foundAlive = 0;
    for (auto obj : *scene)
    {
        if (obj->active) foundAlive++;
    }
    currentlyAlive = foundAlive;
    SpawnBatch();
}

EnemySpawner::~EnemySpawner()
{
    for (auto enemy : *scene)
    {
        delete enemy;
    }
}
