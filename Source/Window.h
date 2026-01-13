#pragma once
#include <raylib.h>
#include <string_view>

struct WindowConfig
{
    int width;
    int height;
    std::string_view title;
    int fps;
};

class Window
{
public:
    Window(const WindowConfig& config)
    {
        InitWindow(config.width, config.height, config.title.data());
        SetTargetFPS(config.fps);
    }

    ~Window() noexcept
    {
        CloseWindow();
    }

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
};