#pragma once

#include <SDL3/SDL.h>
#include <string>

class Texture
{
public:
    Texture();
    ~Texture();

    bool loadFromFile(std::string path);
    bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

    void destroy();

    void setColor(Uint8 r, Uint8 g, Uint8 b);
    void setAlpha(Uint8 alpha);
    void setBlending(SDL_BlendMode blendMode);

    void renderAsBackground(float xOnScreen, float yOnScreen);
    void render(const SDL_FRect srcrect, const SDL_FRect dstrect, double angle);

    SDL_Point getSize() const { return SDL_Point{ mWidth, mHeight }; }
    int getWidth();
    int getHeight();
    bool isLoaded();

    operator SDL_Texture* () { return mTexture; }

private:
    SDL_Texture* mTexture;

    int mWidth;
    int mHeight;
};