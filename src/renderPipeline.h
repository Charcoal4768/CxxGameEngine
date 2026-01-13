#pragma once
#include <SDL.h>
#include <vector>
#include <algorithm>

struct DrawRequest {
    SDL_Rect rect;
    SDL_Color color;
    int zIndex;
    
    SDL_Texture* texture = nullptr; 
    double angle = 0.0;
    SDL_RendererFlip flip = SDL_FLIP_NONE;

    bool operator<(const DrawRequest& other) const {
        return zIndex < other.zIndex;
    }
};
class RenderPipeline {
public:
    void submit(DrawRequest request);
    void execute(SDL_Renderer* renderer);

private:
    std::vector<DrawRequest> requestQueue;
};