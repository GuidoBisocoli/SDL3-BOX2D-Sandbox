#pragma once

#include "Sandbox.h"

#include "box2d/math_functions.h"

// static so I can add include them in any file without conflicts
static float radiansToDegrees(float angle) { return angle * 180 / B2_PI; }
static float floatInBox2DWorld(const float f) { return f / SCALE; }
static b2Vec2 vecInBox2DWorld(const int x, const int y) {
	return b2Vec2{ (float)x / SCALE, (float)y / SCALE };
}
static b2Vec2 vecInBox2DWorld(const float x, const float y) {
	return b2Vec2{ x / SCALE, y / SCALE };
}
static SDL_FPoint vecToSDL_FPoint(b2Vec2 v) {
	return SDL_FPoint{ SCALE * v.x, SCALE * v.y };
}