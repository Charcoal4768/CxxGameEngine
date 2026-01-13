// #include <iostream>
#include "renderPipeline.h"

void RenderPipeline::submit(DrawRequest request){
    requestQueue.push_back(request);
}

void RenderPipeline::execute(SDL_Renderer* renderer) {
    std::stable_sort(requestQueue.begin(), requestQueue.end());

    for (const auto& req : requestQueue) {
        if (req.texture != nullptr) {
            // Use CopyEx for rotation and flipping support
            // Passing nullptr for 'srcrect' uses the whole texture
            // Passing nullptr for 'center' rotates around the middle of req.rect
            SDL_RenderCopyEx(renderer, req.texture, nullptr, &req.rect, req.angle, nullptr, req.flip);
        } else {
            // Fallback for objects that are just rects
            SDL_SetRenderDrawColor(renderer, req.color.r, req.color.g, req.color.b, req.color.a);
            SDL_RenderFillRect(renderer, &req.rect);
        }
    }
    requestQueue.clear();
}
