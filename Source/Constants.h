#pragma once

namespace Constant
{
	namespace Wall
	{
		constexpr int amount{ 5 };
		constexpr int positionOffset{ 250 };
	}

	constexpr int starCount{ 600 };


	namespace Window
	{
		constexpr int Width = 1920;
		constexpr int Height = 1080;
		constexpr int fps = 60;
	}

	namespace EnemyFormation
	{
		constexpr int Width = 8;
		constexpr int Height = 5;
		constexpr int Spacing = 80;
		constexpr int XCord = 100;
		constexpr int YCord = 50;
	}

	namespace UI
	{
		constexpr Rectangle textBox{ 600, 500, 225, 50 };
		constexpr float LineThickness{ 1.0f };
	}

}