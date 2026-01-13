#pragma once
#include "Window.h"
namespace Constant
{
	namespace Wall
	{
		constexpr int amount{ 5 };
		constexpr int positionOffset{ 250 };
	}

	constexpr int starCount{ 600 };

	namespace Window // TODO: improve
	{
		constexpr int Width = 1920;
		constexpr int Height = 1080;
		constexpr int fps = 60;
		constexpr WindowConfig Config{
		Width,
		Height,
		"SPACE INVADERS",
		fps
		};
	}

	namespace EnemyFormation
	{
		constexpr int Width = 8;
		constexpr int Height = 5;
		constexpr int Spacing = 80;
		constexpr int XCord = 100;
		constexpr int YCord = 50;
		constexpr int startXOffset = 450; 
		constexpr int rowDrop{ 50 };
	}

	namespace Projectile
	{
		constexpr int speed{ 15 };
		constexpr int alienShootOffset{ 15 };
		constexpr int playerShootOffset{ -130 };
		constexpr int lineHalfLength{ 15 };
		constexpr int offscreenLimit{ 1500 };
		constexpr int srcSize{ 176 };
		constexpr int renderSize{ 50 };
		constexpr int renderOrigin{ 25 };
	}

	namespace Direction
	{
		constexpr int up{ 1 };
		constexpr int down{ -1 };
	}

	namespace Texture
	{
		constexpr int playerSrcSize{ 352 };
		constexpr int playerRenderSize{ 100 };
		constexpr int playerOrigin{ 50 };

		constexpr int alienSrcSize{ 352 };
		constexpr int alienRenderSize{ 100 };
		constexpr int alienOrigin{ 50 };

		constexpr int wallSrcSize{ 704 };
		constexpr int wallRenderSize{ 200 };
		constexpr int wallOrigin{ 100 };
	}

	
	

	namespace Background
	{
		constexpr int offsetDivisor{ 15 };
		constexpr int initMargin{ 150 };
		constexpr int starSizeMin{ 1 };
		constexpr int starSizeMax{ 4 };
		constexpr int starSizeDivisor{ 2 };
	}

	namespace Score
	{
		constexpr int alienKill{ 100 };
	}

	namespace Animation
	{
		constexpr float frameDuration{ 0.4f };
		constexpr int playerMaxTextureIndex{ 2 };
	}

	namespace UI
	{
		constexpr Rectangle textBox{ 600, 500, 225, 50 };
		constexpr float LineThickness{ 1.0f };

		constexpr int maxNameLength{ 8 };

		constexpr int continueTextX{ 600 };
		constexpr int continueTextY{ 200 };

		namespace Leaderboard
		{
		constexpr int TitleX{ 50 };
		constexpr int TitleY{ 100 };
		constexpr int NameX{ 50 };
		constexpr int StartY{ 140 };
		constexpr int RowSpacing{ 40 };
		constexpr int ScoreX{ 350 };
		}

		constexpr int newHighScoreX{ 600 };
		constexpr int newHighScoreY{ 300 };
		constexpr int inputBoxInfoX{ 600 };
		constexpr int inputBoxInfoY{ 400 };
		constexpr int inputCharsX{ 600 };
		constexpr int inputCharsY{ 600 };
		constexpr int inputWarnX{ 600 };
		constexpr int inputWarnY{ 650 };
		constexpr int enterContinueX{ 600 };
		constexpr int enterContinueY{ 800 };

		namespace FontSize
		{
			constexpr int Small{ 20 };
			constexpr int Medium{ 40 };
			constexpr int Large{ 60 };
			constexpr int VeryLarge{ 160 };
		}
	}
}