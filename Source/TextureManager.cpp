#include "TextureManager.h"

TextureManager::TextureManager() noexcept
    : texture{}
{}

TextureManager::TextureManager(const std::string_view path)
    : texture{} 
{
    texture = LoadTexture(path.data());
}

TextureManager::~TextureManager() noexcept
{
    if (texture.id != 0) { // TODO: Add error handling
        UnloadTexture(texture);
    }
}

TextureManager::TextureManager(TextureManager&& other) noexcept
    : texture{ other.texture }
{
    other.texture = {};
}

TextureManager& TextureManager::operator=(TextureManager&& other) noexcept
{
    if (this != &other) {
        if (texture.id != 0)
        {
            UnloadTexture(texture);
        }
        texture = other.texture;
        other.texture = {};
    }
    return *this;
}

const Texture2D& TextureManager::Get() const noexcept { return texture; }
