#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>
#include "Constants.h"

enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

enum struct EntityType
{
	PLAYER_PROJECTILE,
	ENEMY_PROJECTILE
};

struct PlayerData
{
	std::string name;
	int score;
};

struct Player
{

	float x_pos = 0;
	float speed = Constant::Player::speed;
	float player_base_height = Constant::Player::baseHeight;
	float radius = Constant::Player::radius;
	int lives = Constant::Player::lives;
	int direction = 0;
	int activeTexture = Constant::Player::activeTexture;
	float timer = 0;

	void Initialize();
	void Render(Texture2D texture);
	void Update();

};

struct Projectile
{
	Vector2 position = { 0,0 };
	int speed = Constant::Projectile::speed;
	bool active = true;
	EntityType type = {};

	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

	void Update();

	void Render(Texture2D texture);
};

struct Wall
{
	Vector2 position;
	Rectangle rec;
	bool active;
	Color color;
	int health = Constant::Wall::defaultHealth;
	int radius = Constant::Wall::defaultRadius;


	void Render(Texture2D texture);
	void Update() noexcept;
};

struct Alien
{

	Color color = WHITE;
	Vector2 position = { 0, 0 };
	int x = 0;
	int y = 0;
	float radius = Constant::Alien::radius;
	bool active = true;
	bool moveRight = true;

	int speed = Constant::Alien::speed;

	void Update() noexcept;
	void Render(Texture2D texture);
};


struct Star
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };
	Color color = GRAY;
	float size = 0;
	void Update(float starOffset) noexcept;
	void Render() noexcept;
};

struct Background
{
	void Render() noexcept;
	void Initialize(int starAmount);
	void Update(float offset) noexcept;


	std::vector<Star> Stars;


};

class Game
{
public:
	explicit Game(State state = State{}) noexcept : gameState(state) {}

	State gameState = {};

	void HandleInput();
	void Update();
	void Render();

private:
	void Start();
	void End();
	void Continue() noexcept;
	void UpdateProjectiles();
	void UpdateWalls();
	void UpdateAliens();
	void EraseInactiveEntities();
	void HandleLoseConditions();
	void ResolveCollisions();
	void UpdateGameplayLogic();
	void HandlePlayerProjectileCollisions(Projectile& proj);
	void HandleEnemyProjectileCollisions(Projectile& proj) noexcept;
	void HandleProjectileWallCollisions(Projectile& proj);
	void RenderLeaderboardMenu() noexcept;
	void RenderNameInputMenu() noexcept;
	void RenderGameplay();
	void RenderStartScreen() noexcept;
	void SpawnWalls();
	void SpawnAliens();
	bool CheckNewHighScore();
	void InsertNewHighScore(std::string name);

	Resources resources{};
	Player player{};
	std::vector<Projectile> Projectiles{};
	std::vector<Wall> Walls{};
	std::vector<Alien> Aliens{};
	std::vector<PlayerData> Leaderboard{
		{"Player 1", 500},
		{"Player 2", 400},
		{"Player 3", 300},
		{"Player 4", 200},
		{"Player 5", 100}
	};
	Background background{};

	Vector2 playerPos{};
	Vector2 alienPos{};
	Vector2 cornerPos{};
	float offset{};
	float shootTimer{};
	std::string name{};

	bool mouseOnText{};
	bool newHighScore{};
	int framesCounter{};
	int score{};
};