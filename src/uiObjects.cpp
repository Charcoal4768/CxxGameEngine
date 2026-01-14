#include "uiObjects.h"
#include <cmath>

NoCollRectangle::NoCollRectangle(float _x, float _y, float _width, float _height, SDL_Color _color, int _z)
{
    x = _x;
    y = _y;
    w = _width;
    h = _height;
    color = _color;
    zIndex = _z;
    collidable = false;

    rect = {(int)x, (int)y, (int)w, (int)h};
}

HpTracker::~HpTracker()
{
    delete background;
    delete foreground;
}

HpTracker::HpTracker(float _x, float _y,
                     float _w, float _h,
                     int _b, gameObject *_target,
                     int _bMargin_x, int _bMargin_y,
                     SDL_Color _bgColor, SDL_Color _fgColor)
{
    target_body = _target;
    target_damagebale = dynamic_cast<Damageable *>(_target);
    displayedHp = (float)target_damagebale->hp;
    backgroundMargin_x = _bMargin_x;
    backgroundMargin_y = _bMargin_y;

    background = new NoCollRectangle(_x, _y,
                                     _w + (_bMargin_x * 2), _h + (_bMargin_y * 2),
                                     _bgColor, 100);

    foreground = new NoCollRectangle(_x + (_bMargin_x), _y + (_bMargin_y),
                                     _w, _h,
                                     _fgColor, 101);
    collidable = false;
}

void HpTracker::submitRenderRequest(RenderPipeline &pipeline, int overrideZ)
{
    if (!active)
        return;
    background->submitRenderRequest(pipeline, overrideZ);
    foreground->submitRenderRequest(pipeline, overrideZ);
}

void HpTracker::update(float dt)
{
    if (!target_body || (target_body && !target_body->active))
    {
        this->active = false;
        return;
    }
    if (target_body && target_damagebale)
    {
        // 1. Interpolate displayedHp towards actual hp
        // formula: current + (target - current) * speed * dt
        float targetHp = (float)target_damagebale->hp;
        float diff = targetHp - displayedHp;

        if (fabs(diff) < 0.05f)
        {
            displayedHp = targetHp;
        }
        else
        {
            float t = fabs(diff) / target_damagebale->maxHp;
            t = SDL_clamp(t, 0.0f, 1.0f);

            float ease = 1.0f - powf(1.0f - t, 3.0f);

            displayedHp += diff * ease * lerpSpeed * dt;
        }

        // 2. Update background position (centered above head)
        background->rect = {
            static_cast<int>(target_body->x + (target_body->w / 2) - (background->w / 2)),
            static_cast<int>(target_body->y - background->h - (backgroundMargin_y * 3)),
            static_cast<int>(background->w),
            static_cast<int>(background->h)};

        // 3. Update foreground width using displayedHp
        float hpPercentage = displayedHp / target_damagebale->maxHp;
        // Clamp percentage so it doesn't go negative or over 100% visually
        if (hpPercentage < 0)
            hpPercentage = 0;

        foreground->rect = {
            static_cast<int>(background->rect.x + backgroundMargin_x),
            static_cast<int>(background->rect.y + backgroundMargin_y),
            static_cast<int>(hpPercentage * (background->w - backgroundMargin_x * 2)),
            static_cast<int>(foreground->h)};
    }
}

FeulBar::~FeulBar()
{
    delete background;
    delete foreground;
}

FeulBar::FeulBar(float _x, float _y,
                 float _w, float _h,
                 int _marginX, int _marginY,
                 SDL_Color _bgColor, SDL_Color _fgColor,
                 double &_feul) : feul(_feul)
{
    maxFuel = _feul;
    maxWidth = _w;
    backgroundMargin_x = _marginX;
    backgroundMargin_y = _marginY;
    backgroundColor = _bgColor;
    foregroundColor = _fgColor;

    background = new NoCollRectangle(_x, _y,
                                     _w + (backgroundMargin_x * 2), _h + (backgroundMargin_y * 2),
                                     backgroundColor, 100);

    foreground = new NoCollRectangle(_x + (backgroundMargin_x), _y + (backgroundMargin_y),
                                     _w, _h,
                                     foregroundColor, 101);
    collidable = false;
}

void FeulBar::update(float dt)
{
    float diff = feul - displayedFeul;

    if (fabs(diff) < 0.1f)
    {
        displayedFeul = feul;
    }
    if (maxFuel-feul < 0.2f)
    {
        displayedFeul = maxFuel;
    }
    else
    {
        float t = fabs(diff) / maxFuel;
        t = SDL_clamp(t, 0.0f, 1.0f);

        float ease = 1.0f - powf(1.0f - t, 3.0f);

        displayedFeul += diff * ease * lerpSpeed * dt;
        if (displayedFeul > (0.95*maxFuel)) displayedFeul += lerpSpeed*dt*diff;
    }

    background->rect = {
        static_cast<int>(background->rect.x),
        static_cast<int>(background->rect.y),
        static_cast<int>(background->w),
        static_cast<int>(background->h)};

    float feulPercentage = displayedFeul / maxFuel;
    float displayedHeight = (feulPercentage * (background->h - (backgroundMargin_y*2)));
    if (feulPercentage< 0)
        feulPercentage = 0;

    foreground->rect = {
        static_cast<int>(background->rect.x + backgroundMargin_x),
        static_cast<int>((background->rect.y)+(background->h-(displayedHeight)-backgroundMargin_y)),
        static_cast<int>(foreground->w),
        static_cast<int>(displayedHeight)};
}

void FeulBar::submitRenderRequest(RenderPipeline &pipeline, int overrideZ)
{
    if (!active)
        return;
    background->submitRenderRequest(pipeline, overrideZ);
    foreground->submitRenderRequest(pipeline, overrideZ);
}