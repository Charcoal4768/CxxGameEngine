#include "basicEntity.h"
#include "environmentObject.h"

struct Wall : public gameObject{
    Wall(float _x, float _y, float _width = 4, float _height = 50, SDL_Color _color = {55, 55, 55, 255}, int _z = 1);
};

struct Lava : public EnvironmentObject, public Damaging {
    Lava(float _x, float _y, float _w, float _h, int _z = 3);
};