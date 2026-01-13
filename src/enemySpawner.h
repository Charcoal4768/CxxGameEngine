#include "basicEntity.h"

struct Asteroid:public gameObject, public Moveable, public Damageable, public Damaging{
    float flashWindow = 0.1f;
    Asteroid(float _x, float _y, float _w = 16, float _h = 16, SDL_Color _color = {200, 200, 200, 255}, int _z = 0, SDL_Texture* tex = nullptr){
        x = _x;
        y = _y;
        w = _w;
        h = _h;
        color = _color;
        zIndex = _z;
        speed = 150.0f;
        hp = 5;
        maxHp = 5;
        maxCooldown = 1.0f;
        color = _color;
        attackPower = 1;
        fakeFriction = 0.2f;
        rect = {(int)x, (int)y, (int)w, (int)h};
        texture = tex;
    }
    void update(float dt) override;
};

typedef std::vector<gameObject*> EnemyEntityList;

class EnemySpawner{
private:
    /* data */
public:
    EnemySpawner(SDL_Window* win, int marginY, int marginX, EnemyEntityList* eList, int spawnCap, int batchSize, int coolDown);
    virtual ~EnemySpawner();
};
