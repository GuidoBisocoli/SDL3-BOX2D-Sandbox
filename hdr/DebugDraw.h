#pragma once

#include <box2d/box2d.h>
#include "Globals.h"

b2DebugDraw debugDrawer;     // For drawing colliders during debugging

void drawSolidPolygon(b2Transform transform, const b2Vec2* vertices,
    int vertexCount, float radius, b2HexColor color, void* context);

void drawSolidPolygon(b2Transform transform, const b2Vec2* vertices,
    int vertexCount, float radius, b2HexColor color, void* context)
{
    // Get the pixel format
    SDL_Surface* surface = SDL_GetWindowSurface(Window);
    const SDL_PixelFormatDetails* format = SDL_GetPixelFormatDetails(surface->format);
    // Extract RGB
    Uint8 r, g, b;
    SDL_GetRGB(color, format, NULL, &r, &g, &b);
    // Draw a collider rectangle with lines
    SDL_SetRenderDrawColor(Renderer, r, g, b, SDL_ALPHA_OPAQUE);
    for (int i = 0; i < vertexCount; ++i) {
        int next_index = (i + 1 == vertexCount) ? 0 : i + 1;
        b2Vec2 p0 = b2TransformPoint(transform, vertices[i]);
        b2Vec2 p1 = b2TransformPoint(transform, vertices[next_index]);
        float x0 = p0.x * SCALE;
        float y0 = p0.y * SCALE;
        float x1 = p1.x * SCALE;
        float y1 = p1.y * SCALE;
        SDL_RenderLine(Renderer, x0, y0, x1, y1);
    }
}
