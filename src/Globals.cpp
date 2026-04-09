#include "Globals.h"

// Init globals
const unsigned int SCALE = 64.f; // 1m = 64px;

// Instatiate globals
SDL_Window* Window{ nullptr };
SDL_Renderer* Renderer{ nullptr };
TTF_Font* Font{ nullptr };
Texture TextTexture;