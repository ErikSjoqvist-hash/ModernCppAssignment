#pragma once
#include <raylib.h>

class Window
{
public:
    Window(int width, int height, const char* title, int fps)
    {
        InitWindow(width, height, title);
        SetTargetFPS(fps);
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