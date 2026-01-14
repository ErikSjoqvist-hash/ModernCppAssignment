#pragma once
#include <raylib.h>
#include <string_view>
#include "Errors.h"

struct WindowConfig
{
	int width;
	int height;
	std::string_view title;
	int fps;
};

class Window
{
	bool initialized = false;
public:
	Window(const WindowConfig& config)
	{
		Errors::ensurePrecondition(config.width > 0 && config.height > 0, "Window width and height must be positive");
		Errors::ensurePrecondition(config.fps > 0, "Window fps must be positive");
		Errors::ensurePrecondition(!config.title.empty(), "Window title must not be empty");

		InitWindow(config.width, config.height, config.title.data());
		initialized = true;
		SetTargetFPS(config.fps);
	}

	~Window() noexcept
	{
		if (initialized)
		{
			CloseWindow();
		}
	}

	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;

	Window(Window&&) = delete;
	Window& operator=(Window&&) = delete;
};