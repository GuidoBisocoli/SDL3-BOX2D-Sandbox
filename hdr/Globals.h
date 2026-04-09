#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "Texture.h"

// CONSTANTS

extern const unsigned int SCALE;

// GLOBALS

//The window we'll be rendering to
extern SDL_Window* Window;

//The renderer used to draw to the window
extern SDL_Renderer* Renderer;

//Global font
extern TTF_Font* Font;

//The texture we're going to render text to
extern Texture TextTexture;