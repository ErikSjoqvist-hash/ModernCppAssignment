#include "game.h"
#include "Constants.h"
#include <iostream>
#include <vector>
#include <algorithm>

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



		if (IsKeyPressed(KEY_SPACE))
		{
			Projectile newProjectile;
			newProjectile.position.x = player.x_pos;
			newProjectile.position.y = Constant::Window::Height - 130;
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
				constexpr size_t max_name_length = 9;

				int key = GetCharPressed();
				while (key > 0)
				{
					if (key < min_printable_char || key > max_printable_char) {
						key = GetCharPressed();
						continue;
					}

					if (name.size() >= max_name_length) {
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

			if (name.size() > 0 && name.size() < 9 && IsKeyReleased(KEY_ENTER))
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

void Game::Update()
{//TODO: nesting
	//TODO: magic numbers
	//TODO: long function
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
		background.Update(offset / 15);


		Collision();



		shootTimer += 1;
		if (shootTimer > 59)
		{
			int randomAlienIndex = 0;

			if (Aliens.size() > 1)
			{
				randomAlienIndex = GetRandomValue(0, 100) % Aliens.size();
			}

			Projectile newProjectile;
			newProjectile.position = Aliens[randomAlienIndex].position;
			newProjectile.position.y += 40;
			newProjectile.speed = -15;
			newProjectile.type = EntityType::ENEMY_PROJECTILE;
			Projectiles.push_back(newProjectile);
			shootTimer = 0;
		}


		EraseInactiveEntities();

		break;
	case State::ENDSCREEN:


		break;
	default:
		break;
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
// TODO: raw for
	for (int i = 0; i < Projectiles.size(); i++)
	{
		if (Projectiles[i].type == EntityType::PLAYER_PROJECTILE)
		{
			for (int a = 0; a < Aliens.size(); a++)
			{
				if (CheckCollisionCircleLine(Aliens[a].position, Aliens[a].radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
				{
					Projectiles[i].active = false;
					Aliens[a].active = false;
					score += 100;
				}
			}
		}


		if (Projectiles[i].type == EntityType::ENEMY_PROJECTILE)
		{
			if (CheckCollisionCircleLine({ player.x_pos, Constant::Window::Height - player.player_base_height }, player.radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
			{
				Projectiles[i].active = false;
				player.lives -= 1;
			}
		}



		for (int b = 0; b < Walls.size(); b++)
		{
			if (CheckCollisionCircleLine(Walls[b].position, Walls[b].radius, Projectiles[i].lineStart, Projectiles[i].lineEnd))
			{
				Projectiles[i].active = false;
				Walls[b].health -= 1;
			}
		}
	}
}


void Game::Render()
{
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

void Game::RenderLeaderboardMenu()
{
	//TODO: magic numbers
	//TODO: Raw for
	DrawText("PRESS ENTER TO CONTINUE", 600, 200, Constant::UI::FontSize::Medium, YELLOW);

	DrawText("LEADERBOARD", 50, 100, Constant::UI::FontSize::Medium, YELLOW);

	for (int i = 0; i < Leaderboard.size(); i++)
	{
		const char* tempNameDisplay = Leaderboard[i].name.data();
		DrawText(tempNameDisplay, 50, 140 + (i * 40), Constant::UI::FontSize::Medium, YELLOW);
		DrawText(TextFormat("%i", Leaderboard[i].score), 350, 140 + (i * 40), Constant::UI::FontSize::Medium, YELLOW);
	}
}

void Game::RenderNameInputMenu()
{
	//TODO: magic numbers
	//TODO: long function
	//TODO: casts
	//TODO: nesting
	DrawText("NEW HIGHSCORE!", 600, 300, Constant::UI::FontSize::Large, YELLOW);



	DrawText("PLACE MOUSE OVER INPUT BOX!", 600, 400, Constant::UI::FontSize::Small, YELLOW);

	DrawRectangleRec(Constant::UI::textBox, LIGHTGRAY);
	if (mouseOnText)
	{
		DrawRectangleLinesEx(Constant::UI::textBox, Constant::UI::LineThickness, RED);
	}
	else
	{
		DrawRectangleLinesEx(Constant::UI::textBox, Constant::UI::LineThickness, DARKGRAY);
	}

	DrawText(name.c_str(), (int)Constant::UI::textBox.x + 5, (int)Constant::UI::textBox.y + 8, Constant::UI::FontSize::Medium, MAROON);

	DrawText(TextFormat("INPUT CHARS: %i/%i", name.size(), 8), 600, 600, Constant::UI::FontSize::Small, YELLOW);

	if (mouseOnText)
	{
		if (name.size() < 9)
		{
			if (((framesCounter / 20) % 2) == 0)
			{
				DrawText("_", (int)Constant::UI::textBox.x + 8 + MeasureText(name.c_str(), 40), (int)Constant::UI::textBox.y + 12, Constant::UI::FontSize::Medium, MAROON);
			}

		}
		else
		{
			DrawText("Press BACKSPACE to delete chars...", 600, 650, Constant::UI::FontSize::Small, YELLOW);
		}

	}

	if (name.size() > 0 && name.size() < 9)
	{
		DrawText("PRESS ENTER TO CONTINUE", 600, 800, Constant::UI::FontSize::Medium, YELLOW);
	}
}

void Game::RenderGameplay()
{
	//background render LEAVE THIS AT TOP
	background.Render(); // TODO: render layers

	DrawText(TextFormat("Score: %i", score), 50, 20, Constant::UI::FontSize::Medium, YELLOW);
	DrawText(TextFormat("Lives: %i", player.lives), 50, 70, Constant::UI::FontSize::Medium, YELLOW);


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
	DrawText("SPACE INVADERS", 200, 100, Constant::UI::FontSize::VeryLarge, YELLOW);

	DrawText("PRESS SPACE TO BEGIN", 200, 350, Constant::UI::FontSize::Medium, YELLOW);
}


void Game::SpawnWalls()
{
	constexpr int gapCount{ Constant::Wall::amount + 1 };
	int wall_distance = Constant::Window::Width / gapCount;

	Walls.resize(Constant::Wall::amount);
	int iterator = 1;
	for (std::size_t i = 0; i < Walls.size(); ++i) //TODO: temporary raw for as generate was not working
	{
		Wall wall;
		wall.position.y = Constant::Window::Height - Constant::Wall::positionOffset;
		wall.position.x = wall_distance * iterator++;
		Walls[i] = std::move(wall);
	}
}

void Game::SpawnAliens()
{//TODO: raw for
	//TODO: nesting
	//TODO: object pool
	for (int row = 0; row < Constant::EnemyFormation::Height; row++) {
		for (int col = 0; col < Constant::EnemyFormation::Width; col++) {
			Alien newAlien = Alien();
			newAlien.active = true;
			newAlien.position.x = Constant::EnemyFormation::YCord + 450 + (col * Constant::EnemyFormation::Spacing);
			newAlien.position.y = Constant::EnemyFormation::YCord + (row * Constant::EnemyFormation::Spacing);
			Aliens.push_back(newAlien);
		}
	}

}


bool Game::CheckNewHighScore()
{//TODO: does not need to be a function
	if (score > Leaderboard[4].score)
	{
		return true;
	}

	return false;
}

void Game::InsertNewHighScore(std::string name)
{//TODO: raw for
	//TODO: nesting
	PlayerData newData;
	newData.name = name;
	newData.score = score;

	for (int i = 0; i < Leaderboard.size(); i++)
	{
		if (newData.score > Leaderboard[i].score)
		{

			Leaderboard.insert(Leaderboard.begin() + i, newData);

			Leaderboard.pop_back();

			i = Leaderboard.size();

		}
	}
}


void Player::Initialize()
{

	x_pos = Constant::Window::Width / 2;

}

void Player::Update()
{//TODO: magic numbers

	direction = 0;
	if (IsKeyDown(KEY_LEFT)) //TODO: put in input handler
	{
		direction--;
	}
	if (IsKeyDown(KEY_RIGHT))
	{
		direction++;
	}

	x_pos += speed * direction;

	if (x_pos < 0 + radius)
	{
		x_pos = 0 + radius;
	}
	else if (x_pos > Constant::Window::Width - radius)
	{
		x_pos = Constant::Window::Width - radius;
	}


	timer += GetFrameTime();

	if (timer > 0.4 && activeTexture == 2)
	{
		activeTexture = 0;
		timer = 0;
	}
	else if (timer > 0.4)
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
			352,
			352,
		},
		{
			x_pos, Constant::Window::Height - player_base_height,
			100,
			100,
		}, { 50, 50 },
		0,
		WHITE);
}


void Projectile::Update()
{//TODO: magic numbers
	position.y -= speed;

	lineStart.y = position.y - 15;
	lineEnd.y = position.y + 15;

	lineStart.x = position.x;
	lineEnd.x = position.x;

	if (position.y < 0 || position.y > 1500)
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
			176,
			176,
		},
		{
			position.x,
			position.y,
			50,
			50,
		}, { 25 , 25 },
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
			704,
			704,
		},
		{
			position.x,
			position.y,
			200,
			200,
		}, { 100 , 100 },
		0,
		WHITE);


	DrawText(TextFormat("%i", health), position.x - 21, position.y + 10, Constant::UI::FontSize::Medium, RED);

}


void Alien::Update()
{//TODO: nesting

	if (moveRight)
	{
		position.x += speed;

		if (position.x >= Constant::Window::Width)
		{
			moveRight = false;
			position.y += 50;
		}
	}
	else
	{
		position.x -= speed;

		if (position.x <= 0)
		{
			moveRight = true;
			position.y += 50;
		}
	}
}

void Alien::Render(Texture2D texture)
{//TODO: magic numbers



	DrawTexturePro(texture,
		{
			0,
			0,
			352,
			352,
		},
		{
			position.x,
			position.y,
			100,
			100,
		}, { 50 , 50 },
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
	DrawCircle((int)position.x, (int)position.y, size, color);
}


void Background::Initialize(int starAmount)
{//TODO: raw for
	//TODO: magic values
	//TODO: should starAmount constant be used directly?
	for (int i = 0; i < starAmount; i++)
	{
		Star newStar;

		newStar.initPosition.x = GetRandomValue(-150, Constant::Window::Width + 150);
		newStar.initPosition.y = GetRandomValue(0, Constant::Window::Height);

		//random color?
		newStar.color = SKYBLUE;

		newStar.size = GetRandomValue(1, 4) / 2;

		Stars.push_back(newStar);

	}
}

void Background::Update(float offset)
{//TODO: raw for
	for (int i = 0; i < Stars.size(); i++)
	{
		Stars[i].Update(offset);
	}

}

void Background::Render()
{//TODO: raw for
	for (int i = 0; i < Stars.size(); i++)
	{
		Stars[i].Render();
	}
}