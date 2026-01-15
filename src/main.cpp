#include <SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "renderPipeline.h"
#include "basicEntity.h"
#include "environmentObject.h"
#include "advancedObjects.h"
#include "uiObjects.h"
#include "enemySpawner.h"
#include <vector>
#include <functional>
#include <map>
#include <string>

std::vector<gameObject *> sceneObjects;
enum class GameState
{
    Menu = 0,
    InGame = 1,
    Paused = 2
};
using ControllerFunc = std::function<void(const SDL_Event &)>;
std::map<GameState, ControllerFunc> controllers;
GameState currentState = GameState::InGame;

SDL_Texture *loadTexture(const std::string &path, SDL_Renderer *renderer)
{
    SDL_Texture *newTexture = IMG_LoadTexture(renderer, path.c_str());
    if (newTexture == NULL)
    {
        // Use SDL_GetError() to find out why it failed
        std::cout << "Failed to load texture from file " << path << "! SDL_image Error: " << SDL_GetError() << std::endl;
    }
    return newTexture;
}

struct Player : public gameObject, public Moveable, public Damageable, public Damaging
{
    SDL_Window *window;
    SDL_Color baseColor;
    float flashWindow = 0.2f;
    float radians = 0.0f;
    float degrees = 0.0f;
    int maxFuel;
    double fuel;
    float maxFuelRegenRate; // fuel per second
    float maxFuelCooldown = 1.5f;
    float fuelCooldown = 0.0f;
    float fuelBurnRate; // fuel per second when thrusting
    float maxSpeed = 300.0f;

    Player(float _x, float _y, float _w = 16, float _h = 32, SDL_Color _color = {0, 0, 255, 255}, int _z = 0, SDL_Window *win = nullptr, SDL_Renderer *renderer = nullptr)
    {
        x = _x;
        y = _y;
        w = _w;
        h = _h;
        color = _color;
        zIndex = _z;
        speed = 350.0f;
        window = win;
        hp = 10;
        maxHp = 10;
        maxCooldown = 2.0f;
        baseColor = color;
        attackPower = 2;
        fakeFriction = 0.5f;
        maxFuel = 10;
        fuel = (double)maxFuel;
        maxFuelRegenRate = 1.0f;
        fuelBurnRate = 1.5f;
        rect = {(int)x, (int)y, (int)w, (int)h};
        if (renderer)
            texture = loadTexture("assets/textures/Untitled.png", renderer);
    }
    void flashRedOnDamage()
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
    void feulRegenCheck(float dt)
    {
        if (fuel < maxFuel)
        {
            fuelCooldown -= dt;

            if (fuelCooldown <= 0.0f)
            {
                fuel += (double)maxFuelRegenRate;

                if (fuel > maxFuel)
                    fuel = (double)maxFuel;
                if (maxFuel - fuel < 0.2f)
                {
                    fuel = maxFuel;
                }

                fuelCooldown = maxFuelCooldown;

                // std::cout << "Fuel Regenerated! Current: " << fuel << "\n";
            }
        }
        else
        {
            fuelCooldown = maxFuelCooldown;
        }
    }
    void update(float dt) override
    {
        bool isThrusting = (accX != 0.0f || accY != 0.0f);

        if (isThrusting)
        {
            fuelCooldown = maxCooldown;
        }
        else
        {
            feulRegenCheck(dt);
        }

        // Existing update logic
        applyMovement(x, y, dt);

        // Flash red on damage logic..
        flashRedOnDamage();

        rect = {static_cast<int>(x), static_cast<int>(y), static_cast<int>(w), static_cast<int>(h)};
        angle = degrees;
    }
    void applyMovement(float &x, float &y, float dt) override
    {
        if (fuel <= 0)
        {
            accX = 0;
            accY = 0;
        }
        velX += accX * dt;
        velY += accY * dt;

        float drag = pow(fakeFriction, dt);

        if (sqrt(velX * velX + velY * velY) > maxSpeed)
        {
            float angle = atan2(velY, velX);
            velX = cos(angle) * maxSpeed;
            velY = sin(angle) * maxSpeed;
        }

        velX *= drag;
        velY *= drag;

        x += velX * dt;
        y += velY * dt;

        if (abs(accX) > 0.0f || abs(accY) > 0.0f)
            fuel -= ((sqrt(velX * velX + velY * velY) + 45.0f) * fuelBurnRate / speed) * dt; // Consume fuel based on speed but only when accelerating
        if (fuel < 0.5)
            fuel = 0.0f;
    }
};

CollisionGrid colgrid;

void collisionCheck(double deltaTime)
{
    for (auto obj : sceneObjects)
    {
        if (!obj->active)
            continue;
        if (!obj->collidable)
            continue;

        // 3. Check against nearby objects
        std::vector<gameObject *> potentialColliders = colgrid.getNearby(obj);
        for (auto other : potentialColliders)
        {
            if (obj == other)
                continue; // Don't collide with self

            SDL_Rect intersection;
            if (SDL_IntersectRect(&obj->rect, &other->rect, &intersection))
            {
                Moveable *mover = dynamic_cast<Moveable *>(obj);
                Damaging *hazard = dynamic_cast<Damaging *>(other);
                Damageable *victim = dynamic_cast<Damageable *>(obj);
                if (!mover)
                    continue;
                if (hazard && victim)
                    victim->takeDamage(hazard);

                if (!other->collidable)
                    continue;

                if (intersection.w > intersection.h)
                {
                    if (obj->y < other->y)
                    { // Landing on top
                        obj->y = other->y - (obj->h + 2);
                        mover->velY = -mover->velY * mover->bouncy;
                        // mover->falling = false;
                    }
                    else
                    { // Hitting head
                        obj->y = other->y + (other->h + 2);
                        mover->velY = -mover->velY * mover->bouncy;
                    }
                }
                if (intersection.h > intersection.w)
                {
                    if (obj->x < other->x)
                    { // Hitting from left
                        obj->x = other->x - (obj->w + 4);
                        mover->velX = -mover->velX * mover->bouncy;
                    }
                    else if (obj->x > other->x)
                    { // Hitting from right
                        obj->x = other->x + (other->w + 4);
                        mover->velX = -mover->velX * mover->bouncy;
                    }
                    else
                    {
                        mover->velX = -mover->velX * 2;
                        mover->velY = -mover->velY * 2;
                    }
                }
                // if (victim && victim->damageCooldown > 0)
                // {
                //     victim->damageCooldown -= (float)deltaTime;
                // }
                if (fabs(mover->velX) < 0.1f)
                    mover->velX = 0;
                if (fabs(mover->velY) < 0.1f)
                    mover->velY = 0;
            }
        }
    }
}

void damageableCooldowns(double deltaTime)
{
    for (auto obj : sceneObjects)
    {
        Damageable *victim = dynamic_cast<Damageable *>(obj);
        if (victim && victim->damageCooldown > 0)
        {
            victim->damageCooldown -= (float)deltaTime;
        }
        if (victim && victim->damageCooldown < 0)
        {
            victim->damageCooldown = 0;
        }
    }
}

std::pair<int, int> ABDist(const std::pair<int, int> &A, const std::pair<int, int> &B)
{
    std::pair<int, int> delta = {(A.first - B.first), (A.second - B.second)};
    return delta;
}

int main(int argc, char *argv[])
{
    srand((unsigned int)time(NULL));
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("Failed to initialize SDL2! SDL: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow("Hello World!",
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOW_FULLSCREEN_DESKTOP ? 1920 : 960,
                                       SDL_WINDOW_FULLSCREEN_DESKTOP ? 1080 : 540,
                                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (win == nullptr)
    {
        printf("Failed to create Window! SDL: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG))
    {
        printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
    }

    if (TTF_Init() == -1)
    {
        printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    }

    RenderPipeline pipeline;
    EnemySpawner spawner = EnemySpawner(win, sceneObjects, 225, 225, 25, 10, 10, true);

    Uint64 NOW = SDL_GetPerformanceCounter();
    Uint64 LAST = 0;
    double deltaTime = 0; // in seconds
    double elapsedTime = 0.0;

    // FPS tracking
    int frameCount = 0;
    double fpsTimer = 0.0;
    int displayedFPS = 0;
    double maxDelay = 0.05;
    int mouseX, mouseY;

    bool quit = false;
    bool initializeSpawning = false;
    SDL_Event e;

    Wall *leftWall = new Wall(100.0f, 460.0f, 200.0f, 20.0f, {50, 50, 50, 255}, 1);
    Wall *rightWall = new Wall(500.0f, 100.0f, 100.0f, 100.0f, {50, 200, 50, 255}, 1);
    Player *player = new Player(200.0f, 100.0f, 60.0f, 60.0f, {50, 50, 255, 255}, 2, win, renderer);
    Lava *lavaPit = new Lava(300.0f, 470.0f, 50.0f, 10.0f, 3);
    HpTracker *playerHealthBar = new HpTracker(
        0, 0, // Position (will be overridden by update)
        40, 6,
        0, player,
        2, 2,
        {30, 30, 30, 255},
        {255, 55, 0, 255});
    FeulBar *playerFeulDisplay = new FeulBar(
        20, 40, 10, 600, 4, 4, {100, 100, 100, 55}, {250, 250, 255, 255}, player->fuel);

    // register objects in scene
    sceneObjects.emplace_back(leftWall);
    sceneObjects.emplace_back(rightWall);
    sceneObjects.emplace_back(player);
    sceneObjects.emplace_back(playerHealthBar);
    sceneObjects.emplace_back(playerFeulDisplay);
    sceneObjects.emplace_back(lavaPit);
    std::vector<gameObject*> *enemyScene = spawner.InitializeEnemies();
    for (auto obj : *enemyScene)
    {
        sceneObjects.emplace_back(obj);
    }

    // Controllers: menu does nothing for now, in-game handles movement keys
    controllers[GameState::Menu] = [&](const SDL_Event &ev)
    {
        // menu-specific input handling could go here
    };

    controllers[GameState::InGame] = [&](const SDL_Event &ev)
    {
        switch (ev.type)
        {
        case SDL_MOUSEMOTION:
        {
            SDL_GetMouseState(&mouseX, &mouseY);
            std::pair<int, int> delta = ABDist({player->x + (player->w / 2), player->y + (player->h / 2)}, {mouseX, mouseY});
            float radians = atan2(delta.second, delta.first);
            double degrees = radians * (180.0 / M_PI) - 90;
            // if (degrees < 0) degrees += 360;
            player->radians = radians;
            player->degrees = degrees;
            // std::cout << "Roatation:" << degrees << "\n";
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            // Logic for shooting a bullet could go here!
            break;
        }
        case SDL_KEYDOWN:
        {
            if (ev.key.keysym.scancode == SDL_SCANCODE_P)
            {
                currentState = GameState::Paused;
                break;
            }
            if (ev.key.keysym.scancode == SDL_SCANCODE_D)
            {
                //debug/testing stuff
                spawner.StartSpawning();
                break;
            }
            break;
        }
        }
    };

    controllers[GameState::Paused] = [&](const SDL_Event &ev)
    {
        // pause-specific input handling
    };

    while (!quit)
    {
        LAST = NOW;
        NOW = SDL_GetPerformanceCounter();
        deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());
        elapsedTime += deltaTime;
        if (elapsedTime > 5.0 && (!initializeSpawning))
        {
            initializeSpawning = true;
            spawner.StartSpawning();
        }

        const Uint8 *keystate = SDL_GetKeyboardState(NULL);

        player->accX = 0; // Reset acceleration each frame
        player->accY = 0;

        spawner.Update((float)deltaTime);

        if (keystate[SDL_SCANCODE_SPACE])
        {
            float thrust = player->speed;
            player->accX -= cos(player->radians) * thrust;
            player->accY -= sin(player->radians) * thrust;
        }

        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                quit = true;
            controllers[currentState](e);
        }
        colgrid.clear();

        for (auto obj : sceneObjects)
        {
            if (obj->active)
                colgrid.insert(obj);
            obj->update((float)deltaTime);
            obj->submitRenderRequest(pipeline);
        }
        damageableCooldowns(deltaTime);

        collisionCheck(deltaTime);


        // --- STEP 3: Render ---
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        pipeline.execute(renderer);
        SDL_RenderPresent(renderer);
        if (player->isDed)
        {
            quit = true;
        }
    }

    for (auto obj : sceneObjects)
    {
        delete obj;
    }

    spawner.StopSpawning();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}