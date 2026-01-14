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
	PLAYER,
	ENEMY,
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

	EntityType type = EntityType::PLAYER;

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
	void Update();
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

	EntityType type = EntityType::ENEMY;

	int speed = Constant::Alien::speed;

	void Update();
	void Render(Texture2D texture);
};


struct Star
{
	Vector2 initPosition = { 0, 0 };
	Vector2 position = { 0, 0 };
	Color color = GRAY;
	float size = 0;
	void Update(float starOffset);
	void Render();
};

struct Background
{
	void Render();
	void Initialize(int starAmount);
	void Update(float offset);



	std::vector<Star> Stars;


};

class Game
{
public:
	explicit Game(State state = State{}) : gameState(state) {}
	State gameState = {};



	void HandleInput();
	void Update();
	void Render();

private:
	void Start();
	void End();

	void Continue();


	void UpdateProjectiles();
	void UpdateWalls();
	void UpdateAliens();
	void EraseInactiveEntities();
	void HandleLoseConditions();
	void ResolveCollisions();

	void UpdateStartScreen();
	void UpdateGameplayLogic();
	void UpdateEndScreen();

	void HandlePlayerProjectileCollisions(Projectile& proj);
	void HandleEnemyProjectileCollisions(Projectile& proj);
	void HandleProjectileWallCollisions(Projectile& proj);

	void RenderLeaderboardMenu();

	void RenderNameInputMenu();

	void RenderGameplay();

	void RenderStartScreen();

	void SpawnWalls();
	void SpawnAliens();

	bool CheckNewHighScore();

	void InsertNewHighScore(std::string name);


	Resources resources;

	Player player;

	std::vector<Projectile> Projectiles; 

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;

	std::vector<PlayerData> Leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };

	Background background;



	
	Vector2 playerPos;
	Vector2 alienPos;
	Vector2 cornerPos;
	float offset;

	float shootTimer = 0;


	std::string name{ };

	
	bool mouseOnText = false;
	bool newHighScore = false;

	int framesCounter = 0;
	int score;

};