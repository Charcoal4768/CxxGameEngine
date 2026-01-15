#include "basicEntity.h"

typedef std::vector<gameObject*> EnemyEntityList;
typedef std::vector<Point2D> SpawnPointList;
typedef std::vector<Velocity2D> VelocityTable;

struct Asteroid:public gameObject, public Moveable, public Damageable, public Damaging{
    float flashWindow = 0.1f;
    SDL_Color baseColor;
    int team; // Enemy team
    int xRange, yRange;
    Asteroid(float _x, float _y, float _w = 16, float _h = 16, float _velX = 0, float _velY = 0, SDL_Color _color = {200, 200, 200, 255}, int _z = 0, int _xRange = 0, int _yRange = 0, SDL_Texture* tex = nullptr);
    void update(float dt) override;
    void takeDamage(Damaging* source) override;
    void flashRedOnDamage();
    void respawn(Point2D spawnPoint, float velX, float velY);
};

class EnemySpawner{
private:
    /* data */
    SpawnPointList spawnPoints;
    EnemyEntityList* enemyList;
    std::vector<gameObject*> *scene;
    VelocityTable velocityTable;

    void setupSpawnPoints(SDL_Window* win, int marginY, int marginX, bool edgesOnly, int batchSize);
    void spawnLoop();
    bool edgesOnly;
    int currentlyAlive;
    int maxAliveEnemies;
    int batchSize;
    float coolDownTimer = 0.0f;
    float maxCoolDown;
    std::pair<int,int> windowSize;
public:
    bool paused = true;
    // srand must be called before this constructor to seed randomness, preferably in main()
    EnemySpawner(SDL_Window* win, std::vector<gameObject*> _scene, int marginY, int marginX, int maxAlive, int batchSize, int coolDown, bool edgesOnly = true);
    std::vector<gameObject*> *InitializeEnemies();
    void StartSpawning();
    void StopSpawning();
    void SpawnBatch();
    void Update(float dt);
    virtual ~EnemySpawner();
};
