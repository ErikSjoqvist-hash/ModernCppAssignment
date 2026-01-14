#include "TextureManager.h"
#include "Errors.h"

TextureManager::TextureManager() noexcept
    : texture{}
{}

TextureManager::TextureManager(const std::string_view path) 
    : texture{}
{
    Errors::ensurePrecondition(!path.empty(), "Texture path must not be empty");
    
    const Texture2D tempTexture = LoadTexture(path.data());
    if (tempTexture.id == 0)
    {
        Errors::failResource(std::string("Failed to load texture: ") + std::string(path));
    }
    else
    {
        texture = tempTexture;
    }
}

TextureManager::~TextureManager() noexcept
{
    if (texture.id != 0) { 
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
