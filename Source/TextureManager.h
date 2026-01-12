#pragma once
#include <filesystem>
#include <raylib.h>

class TextureManager
{
public:
    TextureManager() noexcept;
    explicit TextureManager(const std::filesystem::path& path);
    ~TextureManager() noexcept;

    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    TextureManager(TextureManager&& other) noexcept;
    TextureManager& operator=(TextureManager&& other) noexcept;

    const Texture2D& Get() const noexcept;

private:
    Texture2D texture;
};
