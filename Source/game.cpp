#include "game.h"
#include "Constants.h"
#include "Errors.h"
#include <format>
#include <print>
#include <vector>
#include <algorithm>
#include <ranges>

float lineLength(Vector2 A, Vector2 B)
{
	const float length = sqrtf(pow(B.x - A.x, 2) + pow(B.y - A.y, 2));
	return length;
}

void Game::Start()
{
	try {
		SpawnWalls();

		const Player newPlayer;
		player = newPlayer;
		player.Initialize();

		SpawnAliens();

		background.Initialize(Constant::starCount);

		score = 0;

		gameState = State::GAMEPLAY;
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Failed to start game: {}\n", e.what());
		throw;
	}
}

void Game::End()
{
	try {
		Projectiles.clear();
		Walls.clear();
		Aliens.clear();
		newHighScore = CheckNewHighScore();
		gameState = State::ENDSCREEN;
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Error ending game: {}\n", e.what());
		gameState = State::ENDSCREEN;
	}
}

void Game::Continue()
{
	gameState = State::STARTSCREEN;
}

void Game::HandleInput()
{
	try {
		switch (gameState)
		{
		case State::STARTSCREEN:
			if (IsKeyReleased(KEY_SPACE))
			{
				Start();
			}
			break;

		case State::GAMEPLAY:
			if (IsKeyReleased(KEY_Q))
			{
				End();
			}

			player.direction = 0;
			if (IsKeyDown(KEY_LEFT))
			{
				player.direction--;
			}
			if (IsKeyDown(KEY_RIGHT))
			{
				player.direction++;
			}

			if (IsKeyPressed(KEY_SPACE))
			{
				Projectile newProjectile;
				newProjectile.position.x = player.x_pos;
				newProjectile.position.y = Constant::Window::Height + Constant::Projectile::playerShootOffset;
				newProjectile.type = EntityType::PLAYER_PROJECTILE;
				Projectiles.push_back(newProjectile);
			}
			break;

		case State::ENDSCREEN:
			if (IsKeyReleased(KEY_ENTER) && !newHighScore)
			{
				Continue();
			}

			if (newHighScore)
			{
				if (CheckCollisionPointRec(GetMousePosition(), Constant::UI::textBox))
					mouseOnText = true;
				else
					mouseOnText = false;

				if (mouseOnText)
				{
					SetMouseCursor(MOUSE_CURSOR_IBEAM);

					constexpr int min_printable_char = 32;
					constexpr int max_printable_char = 125;

					int key = GetCharPressed();
					while (key > 0)
					{
						if (key < min_printable_char || key > max_printable_char) {
							key = GetCharPressed();
							continue;
						}

						if (name.size() > Constant::UI::maxNameLength) {
							key = GetCharPressed();
							continue;
						}

						name.push_back(static_cast<char>(key));
						key = GetCharPressed();
					}

					if (IsKeyPressed(KEY_BACKSPACE) && name.size() > 0)
					{
						name.pop_back();
					}
				}
				else
					SetMouseCursor(MOUSE_CURSOR_DEFAULT);

				if (mouseOnText)
				{
					framesCounter++;
				}
				else
				{
					framesCounter = 0;
				}

				if (name.size() > 0 && name.size() <= Constant::UI::maxNameLength && IsKeyReleased(KEY_ENTER))
				{
					std::string nameEntry(name);
					InsertNewHighScore(nameEntry);
					newHighScore = false;
				}
			}
			break;

		default:
			break;
		}
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Game error in HandleInput: {}\n", e.what());
		throw;
	}
	catch (const std::exception& e) {
		std::print(stderr, "Unhandled exception in HandleInput: {}\n", e.what());
		throw Errors::GameError(std::format("HandleInput failed: {}", e.what()));
	}
}

void Game::Update()
{
	try {
		
		if (gameState == State::GAMEPLAY)
		{
			UpdateGameplayLogic();

		} 
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Game error in Update: {}\n", e.what());
		throw;
	}
	catch (const std::exception& e) {
		std::print(stderr, "Unhandled exception in Update: {}\n", e.what());
		throw Errors::GameError(std::format("Update failed: {}", e.what()));
	}
}


void Game::UpdateGameplayLogic()
{
	try {
		player.Update();
		UpdateProjectiles();
		UpdateWalls();
		UpdateAliens();
		HandleLoseConditions();

		playerPos = { player.x_pos, player.player_base_height };
		cornerPos = { 0, player.player_base_height };
		offset = lineLength(playerPos, cornerPos) * -1;
		background.Update(offset / Constant::Background::offsetDivisor);

		ResolveCollisions();

		shootTimer++;
		if (shootTimer >= Constant::Window::fps)
		{
			if (!Aliens.empty())
			{
				int randomAlienIndex{ 0 };
				if (Aliens.size() > 1)
				{
					randomAlienIndex = GetRandomValue(0, static_cast<int>(Aliens.size()) - 1);
				}

				Projectile newProjectile;
				newProjectile.position = Aliens[static_cast<std::size_t>(randomAlienIndex)].position;
				newProjectile.position.y += Constant::Projectile::alienShootOffset;
				newProjectile.speed = Constant::Projectile::speed * Constant::Direction::down;
				newProjectile.type = EntityType::ENEMY_PROJECTILE;
				Projectiles.push_back(newProjectile);
			}
			shootTimer = 0;
		}

		EraseInactiveEntities();
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Error in gameplay logic: {}\n", e.what());
		throw;
	}
	catch (const std::exception& e) {
		std::print(stderr, "Unexpected error in gameplay logic: {}\n", e.what());
		throw Errors::GameError(std::format("Gameplay logic failed: {}", e.what()));
	}
}


void Game::UpdateProjectiles()
{
	try {
		std::ranges::for_each(Projectiles, [&](Projectile& projectile) {
			projectile.Update();
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update projectiles: {}", e.what()));
	}
}

void Game::UpdateWalls()
{
	try {
		std::ranges::for_each(Walls, [&](Wall& wall) {
			wall.Update();
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update walls: {}", e.what()));
	}
}

void Game::UpdateAliens()
{
	try {
		std::ranges::for_each(Aliens, [&](Alien& alien) {
			alien.Update();
			});
		if (Aliens.empty())
		{
			SpawnAliens();
		}
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update aliens: {}", e.what()));
	}
}

void Game::EraseInactiveEntities()
{
	try {
		std::erase_if(Projectiles, [&](const Projectile& projectile) {
			return !projectile.active;
			});
		std::erase_if(Aliens, [&](const Alien& alien) {
			return !alien.active;
			});
		std::erase_if(Walls, [&](const Wall& wall) {
			return !wall.active;
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to erase inactive entities: {}", e.what()));
	}
}

void Game::HandleLoseConditions()
{
	try {
		if (std::ranges::any_of(Aliens, [&](const Alien& alien) {
			return alien.position.y > Constant::Window::Height - player.player_base_height;
			})) {
			End();
		}

		if (player.lives < 1)
		{
			End();
		}
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Error checking lose conditions: {}\n", e.what());
	}
}

void Game::ResolveCollisions()
{
	try {
		std::ranges::for_each(Projectiles, [&](Projectile& proj) {
			if (!proj.active) {
				return;
			}

			switch (proj.type)
			{
			case EntityType::PLAYER_PROJECTILE:
				HandlePlayerProjectileCollisions(proj);
				break;
			case EntityType::ENEMY_PROJECTILE:
				HandleEnemyProjectileCollisions(proj);
				break;
			default:
				break;
			}

			HandleProjectileWallCollisions(proj);
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to resolve collisions: {}", e.what()));
	}
}

void Game::HandlePlayerProjectileCollisions(Projectile& proj)
{
	std::ranges::for_each(Aliens, [&](Alien& alien) {
		if (!alien.active)
		{
			return;
		}

		if (CheckCollisionCircleLine(alien.position, alien.radius, proj.lineStart, proj.lineEnd))
		{
			proj.active = false;
			alien.active = false;
			score += Constant::Score::alienKill;
		}
		});
}

void Game::HandleEnemyProjectileCollisions(Projectile& proj)
{
	if (CheckCollisionCircleLine({ player.x_pos, Constant::Window::Height - player.player_base_height },
		player.radius, proj.lineStart, proj.lineEnd))
	{
		proj.active = false;
		player.lives -= 1;
	}
}

void Game::HandleProjectileWallCollisions(Projectile& proj)
{
	std::ranges::for_each(Walls, [&](Wall& wall) {
		if (!wall.active)
		{
			return;
		}
		if (CheckCollisionCircleLine(wall.position, wall.radius, proj.lineStart, proj.lineEnd))
		{
			proj.active = false;
			wall.health -= 1;
		}
		});
}

void Game::Render()
{
	try {
		switch (gameState)
		{
		case State::STARTSCREEN:
			RenderStartScreen();
			break;

		case State::GAMEPLAY:
			RenderGameplay();
			break;

		case State::ENDSCREEN:
			if (newHighScore)
			{
				RenderNameInputMenu();
			}
			else {
				RenderLeaderboardMenu();
			}
			break;

		default:
			break;
		}
	}
	catch (const Errors::GameError& e) {
		std::print(stderr, "Game error in Render: {}\n", e.what());
	}
	catch (const std::exception& e) {
		std::print(stderr, "Unhandled exception in Render: {}\n", e.what());
	}
}

void Game::RenderLeaderboardMenu()
{
	try {
		DrawText("PRESS ENTER TO CONTINUE",
			Constant::UI::continueTextX,
			Constant::UI::continueTextY,
			Constant::UI::FontSize::Medium,
			YELLOW);
		DrawText("LEADERBOARD",
			Constant::UI::Leaderboard::TitleX,
			Constant::UI::Leaderboard::TitleY,
			Constant::UI::FontSize::Medium,
			YELLOW);

		for (const auto& [index, entry] : Leaderboard | std::views::enumerate)
		{
			DrawText(entry.name.c_str(),
				Constant::UI::Leaderboard::NameX,
				Constant::UI::Leaderboard::StartY + (index * Constant::UI::Leaderboard::RowSpacing),
				Constant::UI::FontSize::Medium,
				YELLOW);
			DrawText(TextFormat("%i", entry.score),
				Constant::UI::Leaderboard::ScoreX,
				Constant::UI::Leaderboard::StartY + (index * Constant::UI::Leaderboard::RowSpacing),
				Constant::UI::FontSize::Medium,
				YELLOW);
		}
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render leaderboard: {}", e.what()));
	}
}

void Game::RenderNameInputMenu()
{
	try {
		DrawText("NEW HIGHSCORE!", Constant::UI::newHighScoreX, Constant::UI::newHighScoreY,
			Constant::UI::FontSize::Large, YELLOW);

		DrawText("PLACE MOUSE OVER INPUT BOX!", Constant::UI::inputBoxInfoX, Constant::UI::inputBoxInfoY,
			Constant::UI::FontSize::Small, YELLOW);

		DrawRectangleRec(Constant::UI::textBox, LIGHTGRAY);
		if (mouseOnText)
		{
			DrawRectangleLinesEx(Constant::UI::textBox, Constant::UI::LineThickness, RED);
		}
		else
		{
			DrawRectangleLinesEx(Constant::UI::textBox, Constant::UI::LineThickness, DARKGRAY);
		}

		DrawText(name.c_str(), static_cast<int>(Constant::UI::textBox.x) + Constant::UI::inputTextPaddingX,
			static_cast<int>(Constant::UI::textBox.y) + Constant::UI::inputTextPaddingY,
			Constant::UI::FontSize::Medium, MAROON);

		DrawText(TextFormat("INPUT CHARS: %i/%i", static_cast<int>(name.size()), Constant::UI::maxNameLength),
			Constant::UI::inputCharsX, Constant::UI::inputCharsY, Constant::UI::FontSize::Small, YELLOW);

		if (mouseOnText)
		{
			if (name.size() <= Constant::UI::maxNameLength)
			{
				if (((framesCounter / Constant::UI::cursorBlinkFrames) % 2) == 0)
				{
					DrawText("_", static_cast<int>(Constant::UI::textBox.x) + Constant::UI::inputCursorOffsetX +
						MeasureText(name.c_str(), Constant::UI::FontSize::Medium),
						static_cast<int>(Constant::UI::textBox.y) + Constant::UI::inputCursorOffsetY,
						Constant::UI::FontSize::Medium, MAROON);
				}
			}
			else
			{
				DrawText("Press BACKSPACE to delete chars...", Constant::UI::inputWarnX, Constant::UI::inputWarnY,
					Constant::UI::FontSize::Small, YELLOW);
			}
		}

		if (name.size() > 0 && name.size() <= Constant::UI::maxNameLength)
		{
			DrawText("PRESS ENTER TO CONTINUE", Constant::UI::enterContinueX, Constant::UI::enterContinueY,
				Constant::UI::FontSize::Medium, YELLOW);
		}
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render name input menu: {}", e.what()));
	}
}

void Game::RenderGameplay()
{
	try {
		background.Render();

		DrawText(TextFormat("Score: %i", score), Constant::UI::scoreTextX, Constant::UI::scoreTextY,
			Constant::UI::FontSize::Medium, YELLOW);
		DrawText(TextFormat("Lives: %i", player.lives), Constant::UI::livesTextX, Constant::UI::livesTextY,
			Constant::UI::FontSize::Medium, YELLOW);

		player.Render(resources.shipTextures[player.activeTexture].Get());

		std::ranges::for_each(Projectiles, [&](Projectile& projectile) {
			projectile.Render(resources.laserTexture.Get());
			});

		std::ranges::for_each(Walls, [&](Wall& wall) {
			wall.Render(resources.barrierTexture.Get());
			});

		std::ranges::for_each(Aliens, [&](Alien& alien) {
			alien.Render(resources.alienTexture.Get());
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render gameplay: {}", e.what()));
	}
}

void Game::RenderStartScreen()
{
	try {
		DrawText("SPACE INVADERS", Constant::UI::startTitleX, Constant::UI::startTitleY,
			Constant::UI::FontSize::VeryLarge, YELLOW);

		DrawText("PRESS SPACE TO BEGIN", Constant::UI::startPromptX, Constant::UI::startPromptY,
			Constant::UI::FontSize::Medium, YELLOW);
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render start screen: {}", e.what()));
	}
}

void Game::SpawnWalls()
{
	Errors::ensurePrecondition(Constant::Wall::amount > 0, "Wall amount must be greater than zero");

	try {
		constexpr int gapCount{ Constant::Wall::amount + 1 };
		const int wall_distance = Constant::Window::Width / gapCount;

		Walls = std::views::iota(1, Constant::Wall::amount + 1)
			| std::views::transform([wall_distance](int iterator) {
			Wall wall;
			wall.position.y = Constant::Window::Height - Constant::Wall::positionOffset;
			wall.position.x = wall_distance * iterator;
			return wall;
				})
			| std::ranges::to<std::vector>();

		Errors::ensurePostcondition(Walls.size() == Constant::Wall::amount,
			"Wall count mismatch after spawning");
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to spawn walls: {}", e.what()));
	}
}

void Game::SpawnAliens()
{
	Errors::ensurePrecondition(Constant::EnemyFormation::Width > 0 && Constant::EnemyFormation::Height > 0,
		"Enemy formation dimensions must be positive");

	try {
		const std::size_t expectedCount = Constant::EnemyFormation::Height * Constant::EnemyFormation::Width;
		Aliens.reserve(Aliens.size() + expectedCount);

		std::ranges::for_each(std::views::iota(0, Constant::EnemyFormation::Height), [&](int row) {
			std::ranges::for_each(std::views::iota(0, Constant::EnemyFormation::Width), [&](int col) {
				Alien newAlien = Alien();
				newAlien.active = true;
				newAlien.position.x = Constant::EnemyFormation::YCord + Constant::EnemyFormation::startXOffset +
					(col * Constant::EnemyFormation::Spacing);
				newAlien.position.y = Constant::EnemyFormation::YCord + (row * Constant::EnemyFormation::Spacing);
				Aliens.push_back(newAlien);
				});
			});

		Errors::ensurePostcondition(Aliens.size() >= expectedCount,
			"Alien count mismatch after spawning");
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to spawn aliens: {}", e.what()));
	}
}

bool Game::CheckNewHighScore()
{
	Errors::ensurePrecondition(!Leaderboard.empty(), "Leaderboard must not be empty when checking new high score");

	try {
		return score > Leaderboard[Constant::UI::Leaderboard::lastRowIndex].score;
	}
	catch (const std::out_of_range& e) {
		throw Errors::GameError(std::format("Invalid leaderboard access: {}", e.what()));
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to check high score: {}", e.what()));
	}
}

void Game::InsertNewHighScore(std::string name)
{
	Errors::ensurePrecondition(!Leaderboard.empty(), "Leaderboard must not be empty to insert a new high score");

	try {
		if (name.empty())
		{
			name = "ANON";
		}

		PlayerData newData{ std::move(name), score };

		auto insertion_point = std::ranges::find_if(Leaderboard,
			[&newData](const auto& entry) { return newData.score > entry.score; });

		if (insertion_point != Leaderboard.end())
		{
			Leaderboard.insert(insertion_point, std::move(newData));
			Leaderboard.pop_back();
		}

		Errors::ensurePostcondition(Leaderboard.size() == Constant::UI::Leaderboard::lastRowIndex + 1,
			"Leaderboard size changed unexpectedly");
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to insert high score: {}", e.what()));
	}
}

void Player::Initialize()
{
	try {
		x_pos = static_cast<float>(Constant::Window::Width) / 2;

		Errors::ensurePostcondition(x_pos >= 0 && x_pos <= Constant::Window::Width,
			"Player position out of bounds after initialization");
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to initialize player: {}", e.what()));
	}
}

void Player::Update()
{
	try {
		x_pos += speed * direction;

		if (x_pos < radius)
		{
			x_pos = radius;
		}
		else if (x_pos > Constant::Window::Width - radius)
		{
			x_pos = Constant::Window::Width - radius;
		}

		timer += GetFrameTime();

		if (timer > Constant::Animation::frameDuration && activeTexture == Constant::Animation::playerMaxTextureIndex)
		{
			activeTexture = 0;
			timer = 0;
		}
		else if (timer > Constant::Animation::frameDuration)
		{
			activeTexture++;
			timer = 0;
		}

		Errors::ensureInvariant(x_pos >= 0 && x_pos <= Constant::Window::Width,
			"Player position out of bounds");
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update player: {}", e.what()));
	}
}

void Player::Render(Texture2D texture)
{
	try {
		Errors::ensurePrecondition(texture.id != 0, "Invalid texture provided to Player::Render");

		DrawTexturePro(texture,
			{
				0,
				0,
				Constant::Texture::playerSrcSize,
				Constant::Texture::playerSrcSize,
			},
			{
				x_pos, Constant::Window::Height - player_base_height,
				Constant::Texture::playerRenderSize,
				Constant::Texture::playerRenderSize,
			},
			{ Constant::Texture::playerOrigin, Constant::Texture::playerOrigin },
			0,
			WHITE);
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render player: {}", e.what()));
	}
}

void Projectile::Update()
{
	Errors::ensurePrecondition(Constant::Projectile::lineHalfLength > 0,
		"Projectile::lineHalfLength must be positive");

	try {
		position.y -= speed;

		lineStart.y = position.y - Constant::Projectile::lineHalfLength;
		lineEnd.y = position.y + Constant::Projectile::lineHalfLength;

		lineStart.x = position.x;
		lineEnd.x = position.x;

		if (position.y < 0 || position.y > Constant::Projectile::offscreenLimit)
		{
			active = false;
		}
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update projectile: {}", e.what()));
	}
}

void Projectile::Render(Texture2D texture)
{
	try {
		Errors::ensurePrecondition(texture.id != 0, "Invalid texture provided to Projectile::Render");

		DrawTexturePro(texture,
			{
				0,
				0,
				Constant::Projectile::srcSize,
				Constant::Projectile::srcSize,
			},
			{
				position.x,
				position.y,
				Constant::Projectile::renderSize,
				Constant::Projectile::renderSize,
			},
			{ Constant::Projectile::renderOrigin , Constant::Projectile::renderOrigin },
			0,
			WHITE);
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render projectile: {}", e.what()));
	}
}

void Wall::Update()
{
	if (health < 1)
	{
		active = false;
	}
}

void Wall::Render(Texture2D texture)
{
	try {
		Errors::ensurePrecondition(texture.id != 0, "Invalid texture provided to Wall::Render");

		DrawTexturePro(texture,
			{
				0,
				0,
				Constant::Texture::wallSrcSize,
				Constant::Texture::wallSrcSize,
			},
			{
				position.x,
				position.y,
				Constant::Texture::wallRenderSize,
				Constant::Texture::wallRenderSize,
			},
			{ Constant::Texture::wallOrigin , Constant::Texture::wallOrigin },
			0,
			WHITE);

		DrawText(TextFormat("%i", health), position.x + Constant::UI::wallHealthTextOffsetX,
			position.y + Constant::UI::wallHealthTextOffsetY, Constant::UI::FontSize::Medium, RED);
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render wall: {}", e.what()));
	}
}

void Alien::Update()
{
	try {
		if (moveRight)
		{
			position.x += speed;

			if (position.x >= Constant::Window::Width)
			{
				moveRight = false;
				position.y += Constant::EnemyFormation::rowDrop;
			}
		}
		else
		{
			position.x -= speed;

			if (position.x <= 0)
			{
				moveRight = true;
				position.y += Constant::EnemyFormation::rowDrop;
			}
		}
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update alien: {}", e.what()));
	}
}

void Alien::Render(Texture2D texture)
{
	try {
		Errors::ensurePrecondition(texture.id != 0, "Invalid texture provided to Alien::Render");

		DrawTexturePro(texture,
			{
				0,
				0,
				Constant::Texture::alienSrcSize,
				Constant::Texture::alienSrcSize,
			},
			{
				position.x,
				position.y,
				Constant::Texture::alienRenderSize,
				Constant::Texture::alienRenderSize,
			},
			{ Constant::Texture::alienOrigin , Constant::Texture::alienOrigin },
			0,
			WHITE);
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render alien: {}", e.what()));
	}
}

void Star::Update(float starOffset)
{
	position.x = initPosition.x + starOffset;
	position.y = initPosition.y;
}

void Star::Render()
{
	DrawCircleV(position, size, color);
}

void Background::Initialize(int starAmount)
{
	Errors::ensurePrecondition(starAmount > 0, "starAmount must be positive");

	try {
		Stars.reserve(Stars.size() + starAmount);
		std::generate_n(std::back_inserter(Stars), starAmount, []() {
			Star newStar;
			newStar.initPosition.x = GetRandomValue(-Constant::Background::initMargin,
				Constant::Window::Width + Constant::Background::initMargin);
			newStar.initPosition.y = GetRandomValue(0, Constant::Window::Height);
			newStar.color = SKYBLUE;
			newStar.size = GetRandomValue(Constant::Background::starSizeMin, Constant::Background::starSizeMax) / Constant::Background::starSizeDivisor;
			return newStar;
			});
		Errors::ensurePostcondition(Stars.size() >= static_cast<std::size_t>(starAmount),
			"Star count mismatch after initialization");
	}
	catch (const Errors::GameError&) {
		throw;
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to initialize background: {}", e.what()));
	}
}
void Background::Update(float offset)
{
	try {
		std::ranges::for_each(Stars, [&](Star& s) {
			s.Update(offset);
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to update background: {}", e.what()));
	}
}
void Background::Render()
{
	try {
		std::ranges::for_each(Stars, [&](Star& s) {
			s.Render();
			});
	}
	catch (const std::exception& e) {
		throw Errors::GameError(std::format("Failed to render background: {}", e.what()));
	}
}