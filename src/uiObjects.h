#include "basicEntity.h"

struct NoCollRectangle : public gameObject{
    NoCollRectangle(float _x, float _y, float _width = 4, float _height = 50, SDL_Color _color = {55, 55, 55, 255}, int _z = 4);
    virtual ~NoCollRectangle() = default;
};

struct HpTracker : public gameObject {
    Damageable* target_damagebale;
    gameObject* target_body;
    float displayedHp;
    float lerpSpeed = 5.0f;
    float maxWidth;
    int backgroundMargin_x;
    int backgroundMargin_y;
    SDL_Color backgroundColor;
    SDL_Color foregroundColor;
    NoCollRectangle* background;
    NoCollRectangle* foreground;

    HpTracker(float _x, float _y,
        float _w, float _h, 
        int _b, gameObject* _target, 
        int _bMargin_x, int _bMargin_y,
        SDL_Color _bgColor = {15,15,15,255}, SDL_Color _fgColor = {255,0,0,255});
    void submitRenderRequest(RenderPipeline &pipeline, int overrideZ = -1) override;
    void update(float dt) override;
    virtual ~HpTracker();
};

struct FeulBar: public gameObject {
    float dissapearTime = 2.0f; //start fading away after 2 seconds of no fuel usage
    float timeSinceUse = 0.0f;
    double maxFuel;
    double &feul;
    float displayedFeul;
    float lerpSpeed = 5.0f;
    float maxWidth;
    int backgroundMargin_x;
    int backgroundMargin_y;
    SDL_Color backgroundColor;
    SDL_Color foregroundColor;
    NoCollRectangle* background;
    NoCollRectangle* foreground;

    FeulBar(float _x, float _y,
        float _w, float _h,
        int _marginX, int _marginY,
        SDL_Color _bgColor, SDL_Color _fgcolor,
        double &_feul);
    void submitRenderRequest(RenderPipeline &pipeline, int overrideZ = -1) override;
    void update(float dt) override;
    virtual ~FeulBar();
};