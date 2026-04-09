#include "Texture.h"

#include <SDL3_image/SDL_image.h>

#include "Globals.h"

Texture::Texture() : mTexture{ nullptr }, mWidth{ 0 }, mHeight{ 0 }
{

}

Texture::~Texture()
{
    destroy();
}

bool Texture::loadFromFile(std::string path)
{
    destroy();

    if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr)
    {
        SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
    }
    else
    {
        if (SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0x00, 0xFF, 0xFF)) == false)
        {
            SDL_Log("Unable to color key! SDL error: %s", SDL_GetError());
        }
        else
        {
            if (mTexture = SDL_CreateTextureFromSurface(Renderer, loadedSurface); mTexture == nullptr)
            {
                SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
            }
            else
            {
                mWidth = loadedSurface->w;
                mHeight = loadedSurface->h;
            }
        }

        SDL_DestroySurface(loadedSurface);
    }

    return mTexture != nullptr;
}

bool Texture::loadFromRenderedText(std::string textureText, SDL_Color textColor)
{
    destroy();

    if (SDL_Surface* textSurface = TTF_RenderText_Blended(Font, textureText.c_str(), 0, textColor); textSurface == nullptr)
    {
        SDL_Log("Unable to render text surface! SDL_ttf Error: %s\n", SDL_GetError());
    }
    else
    {
        if (mTexture = SDL_CreateTextureFromSurface(Renderer, textSurface); mTexture == nullptr)
        {
            SDL_Log("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        }
        else
        {
            mWidth = textSurface->w;
            mHeight = textSurface->h;
        }

        SDL_DestroySurface(textSurface);
    }

    return mTexture != nullptr;
}

void Texture::destroy()
{
    SDL_DestroyTexture(mTexture);
    mTexture = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void Texture::setColor(Uint8 r, Uint8 g, Uint8 b)
{
    SDL_SetTextureColorMod(mTexture, r, g, b);
}

void Texture::setAlpha(Uint8 alpha)
{
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

void Texture::setBlending(SDL_BlendMode blendMode)
{
    SDL_SetTextureBlendMode(mTexture, blendMode);
}

void Texture::renderAsBackground(float xOnScreen, float yOnScreen)
{
    SDL_FRect rect{ 0.f, 0.f, xOnScreen, yOnScreen };

    SDL_RenderTexture(Renderer, mTexture, &rect, NULL);
}

void Texture::render(const SDL_FRect srcrect, const SDL_FRect dstrect, double angle)
{
    SDL_RenderTextureRotated(Renderer, mTexture, &srcrect, &dstrect, angle, NULL, SDL_FLIP_NONE);
}

int Texture::getWidth()
{
    return mWidth;
}

int Texture::getHeight()
{
    return mHeight;
}

bool Texture::isLoaded()
{
    return mTexture != nullptr;
}