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

	SpawnWalls();


	const Player newPlayer;// TODO: make reset player function
	player = newPlayer;
	player.Initialize();


	SpawnAliens();

	background.Initialize(Constant::starCount);


	score = 0;

	gameState = State::GAMEPLAY;

}

void Game::End()
{
	Projectiles.clear();
	Walls.clear();
	Aliens.clear();
	newHighScore = CheckNewHighScore();
	gameState = State::ENDSCREEN;
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
				if (CheckCollisionPointRec(GetMousePosition(), Constant::UI::textBox)) mouseOnText = true;
				else mouseOnText = false;

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
				else SetMouseCursor(MOUSE_CURSOR_DEFAULT);

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
	} catch (const Errors::GameError& e) {
		std::print(stderr, "Game error in HandleInput: {}\n", e.what());
	} catch (const std::exception& e) {
		std::print(stderr, "Unhandled exception in HandleInput: {}\n", e.what());
	}
}

void Game::Update()
{//TODO: nesting
	//TODO: magic numbers
	//TODO: long function
	try {
	switch (gameState)
	{
	case State::STARTSCREEN:


		break;
	case State::GAMEPLAY:


		player.Update();


		UpdateProjectiles();

		UpdateWalls();

		UpdateAliens();

		HandleLoseConditions();


		// TODO: improve
		playerPos = { player.x_pos, player.player_base_height };
		cornerPos = { 0, player.player_base_height };
		offset = lineLength(playerPos, cornerPos) * -1;
		background.Update(offset / Constant::Background::offsetDivisor);


		Collision();



		shootTimer++;
		if (shootTimer >= Constant::Window::fps)
		{
			
			if (!Aliens.empty())
			{
				int randomAlienIndex{0};
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

		break;
	case State::ENDSCREEN:


		break;
	default:
		break;
	}
	} catch (const Errors::GameError& e) {
		std::print(stderr, "Game error in Update: {}\n", e.what());
	} catch (const std::exception& e) {
		std::print(stderr, "Unhandled exception in Update: {}\n", e.what());
	}
}


void Game::UpdateProjectiles()
{
	std::ranges::for_each(Projectiles, [&](Projectile& projectile) { //TODO: inconsisten ampersand placement
		projectile.Update();
		});
}

void Game::UpdateWalls()
{
	std::ranges::for_each(Walls, [&](Wall& wall) {
		wall.Update();
		});
}

void Game::UpdateAliens()
{
	std::ranges::for_each(Aliens, [&](Alien& alien) {
		alien.Update();
		});
	if (Aliens.size() < 1)
	{
		SpawnAliens();
	}
}


void Game::EraseInactiveEntities()
{
	std::erase_if(Projectiles, [&](const Projectile& projectile)
		{
			return !projectile.active;
		});
	std::erase_if(Aliens, [&](const Alien& alien)
		{
			return !alien.active;
		});
	std::erase_if(Walls, [&](const Wall& wall)
		{
			return !wall.active;
		});
}

void Game::HandleLoseConditions()
{
	if (std::ranges::any_of(Aliens, [&](const Alien& alien) {
		return alien.position.y > Constant::Window::Height - player.player_base_height;
		})) {
		End();
	}

	if (player.lives < 1)
	{
		End(); // TODO: do not bother with rest of update after end()
	}
}


void Game::Collision()// TODO: improve name
{// TODO: nesting
	std::ranges::for_each(Projectiles, [&](Projectile& proj)
		{
			if (proj.type == EntityType::PLAYER_PROJECTILE)
			{
				std::ranges::for_each(Aliens, [&](Alien& alien)
					{
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

			if (proj.type == EntityType::ENEMY_PROJECTILE)
			{
				if (CheckCollisionCircleLine({ player.x_pos, Constant::Window::Height - player.player_base_height }, player.radius, proj.lineStart, proj.lineEnd))
				{
					proj.active = false;
					player.lives -= 1;
				}
			}

			std::ranges::for_each(Walls, [&](Wall& wall)
				{
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
	} catch (const Errors::GameError& e) { // TODO: wat
		Errors::throwIfSerious(false, std::string("Game error in Render: ") + e.what());
	} catch (const std::exception& e) {
		Errors::throwIfSerious(false, std::string("Unhandled exception in Render: ") + e.what());
	}
}

void Game::RenderLeaderboardMenu()
{
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

void Game::RenderNameInputMenu()
{
	//TODO: magic numbers
	//TODO: long function
	//TODO: casts
	//TODO: nesting
	DrawText("NEW HIGHSCORE!", Constant::UI::newHighScoreX, Constant::UI::newHighScoreY, Constant::UI::FontSize::Large, YELLOW);
	

	DrawText("PLACE MOUSE OVER INPUT BOX!", Constant::UI::inputBoxInfoX, Constant::UI::inputBoxInfoY, Constant::UI::FontSize::Small, YELLOW);

	DrawRectangleRec(Constant::UI::textBox, LIGHTGRAY);
	if (mouseOnText)
	{
		DrawRectangleLinesEx(Constant::UI::textBox, Constant::UI::LineThickness, RED);
	}
	else
	{
		DrawRectangleLinesEx(Constant::UI::textBox, Constant::UI::LineThickness, DARKGRAY);
	}

	DrawText(name.c_str(), (int)Constant::UI::textBox.x + Constant::UI::inputTextPaddingX, (int)Constant::UI::textBox.y + Constant::UI::inputTextPaddingY, Constant::UI::FontSize::Medium, MAROON); // TODO: magic numbers

	DrawText(TextFormat("INPUT CHARS: %i/%i", name.size(), Constant::UI::maxNameLength), Constant::UI::inputCharsX, Constant::UI::inputCharsY, Constant::UI::FontSize::Small, YELLOW);

	if (mouseOnText)
	{
		if (name.size() <= Constant::UI::maxNameLength)
		{
			if (((framesCounter / Constant::UI::cursorBlinkFrames) % 2) == 0) // TODO: magic numbers
			{
				DrawText("_", (int)Constant::UI::textBox.x + Constant::UI::inputCursorOffsetX + MeasureText(name.c_str(), Constant::UI::FontSize::Medium), (int)Constant::UI::textBox.y + Constant::UI::inputCursorOffsetY, Constant::UI::FontSize::Medium, MAROON); // TODO: magic numbers
			}

		}
		else
		{
			DrawText("Press BACKSPACE to delete chars...", Constant::UI::inputWarnX, Constant::UI::inputWarnY, Constant::UI::FontSize::Small, YELLOW);
		}

	}

	if (name.size() > 0 && name.size() <= Constant::UI::maxNameLength)
	{
		DrawText("PRESS ENTER TO CONTINUE", Constant::UI::enterContinueX, Constant::UI::enterContinueY, Constant::UI::FontSize::Medium, YELLOW);
	}
}

void Game::RenderGameplay()
{
	//background render LEAVE THIS AT TOP
	background.Render(); // TODO: render layers

	DrawText(TextFormat("Score: %i", score), Constant::UI::scoreTextX, Constant::UI::scoreTextY, Constant::UI::FontSize::Medium, YELLOW); // TODO: magic numbers
	DrawText(TextFormat("Lives: %i", player.lives), Constant::UI::livesTextX, Constant::UI::livesTextY, Constant::UI::FontSize::Medium, YELLOW); // TODO: magic numbers


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

void Game::RenderStartScreen()
{
	DrawText("SPACE INVADERS", Constant::UI::startTitleX, Constant::UI::startTitleY, Constant::UI::FontSize::VeryLarge, YELLOW); // TODO: magic numbers

	DrawText("PRESS SPACE TO BEGIN", Constant::UI::startPromptX, Constant::UI::startPromptY, Constant::UI::FontSize::Medium, YELLOW); // TODO: magic numbers
}


void Game::SpawnWalls()
{
	Errors::ensurePrecondition(Constant::Wall::amount > 0, "Wall amount must be greater than zero");

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
}

void Game::SpawnAliens()
{
	Errors::ensurePrecondition(Constant::EnemyFormation::Width > 0 && Constant::EnemyFormation::Height > 0, "Enemy formation dimensions must be positive");

	//TODO: nesting
	//TODO: object pool
	Aliens.reserve(Aliens.size() + (Constant::EnemyFormation::Height * Constant::EnemyFormation::Width));
	std::ranges::for_each(std::views::iota(0, Constant::EnemyFormation::Height), [&](int row) {
		std::ranges::for_each(std::views::iota(0, Constant::EnemyFormation::Width), [&](int col) {
			Alien newAlien = Alien();
			newAlien.active = true;
			newAlien.position.x = Constant::EnemyFormation::YCord + Constant::EnemyFormation::startXOffset + (col * Constant::EnemyFormation::Spacing);
			newAlien.position.y = Constant::EnemyFormation::YCord + (row * Constant::EnemyFormation::Spacing);
			Aliens.push_back(newAlien);
			});
		});
}

bool Game::CheckNewHighScore()
{
	Errors::ensurePrecondition(!Leaderboard.empty(), "Leaderboard must not be empty when checking new high score");

	if (score > Leaderboard[Constant::UI::Leaderboard::lastRowIndex].score)
	{
		return true;
	}

	return false;
}

void Game::InsertNewHighScore(std::string name)
{
	Errors::ensurePrecondition(!Leaderboard.empty(), "Leaderboard must not be empty to insert a new high score");

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
}


void Player::Initialize()
{

	x_pos = static_cast<float>(Constant::Window::Width) / 2;

}

void Player::Update()
{

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


}

void Player::Render(Texture2D texture)
{//TODO: magic numbers

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
		}, { Constant::Texture::playerOrigin, Constant::Texture::playerOrigin },
		0,
		WHITE);
}


void Projectile::Update()
{//TODO: magic numbers
	// precondition
	Errors::ensurePrecondition(Constant::Projectile::lineHalfLength > 0, "Projectile::lineHalfLength must be positive");

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

void Projectile::Render(Texture2D texture)
{//TODO: magic numbers
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
		}, { Constant::Projectile::renderOrigin , Constant::Projectile::renderOrigin },
		0,
		WHITE);
}


void Wall::Update()
{

	if (health < 1)
	{
		active = false;
	}


}

void Wall::Render(Texture2D texture)
{//TODO: magic numbers
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
		}, { Constant::Texture::wallOrigin , Constant::Texture::wallOrigin },
		0,
		WHITE);


	DrawText(TextFormat("%i", health), position.x + Constant::UI::wallHealthTextOffsetX, position.y + Constant::UI::wallHealthTextOffsetY, Constant::UI::FontSize::Medium, RED); // TODO: magic numbers

}


void Alien::Update()
{//TODO: nesting

	if (moveRight)
	{
		position.x += speed;

		if (position.x >= Constant::Window::Width)
		{
			moveRight = false;
			position.y += Constant::EnemyFormation::rowDrop; // TODO: move drop to formation
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

void Alien::Render(Texture2D texture)
{//TODO: magic numbers



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
		}, { Constant::Texture::alienOrigin , Constant::Texture::alienOrigin },
		0,
		WHITE);
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
	//TODO: should starAmount constant be used directly?
	Stars.reserve(Stars.size() + starAmount);
	std::generate_n(std::back_inserter(Stars), starAmount, []() {
		Star newStar;
		newStar.initPosition.x = GetRandomValue(-Constant::Background::initMargin, Constant::Window::Width + Constant::Background::initMargin);
		newStar.initPosition.y = GetRandomValue(0, Constant::Window::Height);
		newStar.color = SKYBLUE;
		newStar.size = GetRandomValue(Constant::Background::starSizeMin, Constant::Background::starSizeMax) / Constant::Background::starSizeDivisor;
		return newStar;
		});
}

void Background::Update(float offset)
{
	std::ranges::for_each(Stars, [&](Star& s) {
		s.Update(offset);
		});
}

void Background::Render()
{
	std::ranges::for_each(Stars, [&](Star& s) {
		s.Render();
		});
}