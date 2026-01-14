#pragma once
#include <string_view>
#include <raylib.h>

class TextureManager
{
public:
    TextureManager() noexcept;
    explicit TextureManager(const std::string_view path); 
    ~TextureManager() noexcept;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    TextureManager(TextureManager&& other) noexcept;
    TextureManager& operator=(TextureManager&& other) noexcept;

    const Texture2D& Get() const noexcept;

private:
    Texture2D texture;
};
