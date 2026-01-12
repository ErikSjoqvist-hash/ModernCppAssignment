#include "TextureManager.h"

TextureManager::TextureManager() noexcept
    : texture{}
{}

TextureManager::TextureManager(const std::filesystem::path& path)
    : texture{} 
{
    texture = LoadTexture(path.string().c_str());
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
        if (texture.id != 0) UnloadTexture(texture);
        texture = other.texture;
        other.texture = {};
    }
    return *this;
}

const Texture2D& TextureManager::Get() const noexcept { return texture; }
