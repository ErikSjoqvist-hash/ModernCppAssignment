#pragma once
#include "raylib.h"
#include <vector>
#include "Resources.h"
#include <string>

//TODO: should be enum class
enum struct State
{
	STARTSCREEN,
	GAMEPLAY,
	ENDSCREEN
};

//TODO: should be enum class
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
//TODO: should be class
struct Player
{//TODO: magic numbers
public:

	float x_pos = 0;
	float speed = 7;
	float player_base_height = 70.0f;
	float radius = 50;
	int lives = 3;
	int direction = 0;
	int activeTexture = 0;
	float timer = 0;

	EntityType type = EntityType::PLAYER;

	void Initialize();
	void Render(Texture2D texture);
	void Update();

};

//TODO: should be class
struct Projectile
{//TODO: magic numbers
public:
	// INITIALIZE PROJECTILE WHILE DEFINING IF ITS PLAYER OR ENEMY 
	Vector2 position = { 0,0 };
	int speed = 15;
	bool active = true;
	EntityType type = {};

	// LINE WILL UPDATE WITH POSITION FOR CALCULATIONS
	Vector2 lineStart = { 0, 0 };
	Vector2 lineEnd = { 0, 0 };

	void Update();

	void Render(Texture2D texture);
};
//TODO: should be class
struct Wall
{//TODO: magic numbers
public:
	Vector2 position;
	Rectangle rec;
	bool active;
	Color color;
	int health = 50;
	int radius = 60;


	void Render(Texture2D texture);
	void Update();
};

//TODO: should be class
struct Alien
{//TODO: magic numbers
public:

	Color color = WHITE;
	Vector2 position = { 0, 0 };
	int x = 0;
	int y = 0;
	float radius = 30;
	bool active = true;
	bool moveRight = true;

	EntityType type = EntityType::ENEMY;

	int speed = 2;

	void Update();
	void Render(Texture2D texture);
};

//TODO: should be class

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


	std::vector<Star> Stars;

	void Initialize(int starAmount);
	void Update(float offset);
	void Render();

};

//TODO: should be class
struct Game
{//TODO: magic numbers
	// Gamestate
	State gameState = {};

	// Score
	int score;

	

	//Aliens shooting
	float shootTimer = 0;


	

	bool newHighScore = false;


	void Start();
	void End();

	void Continue();
	void Launch();

	void HandleInput();

	void Update();
	void UpdateProjectiles();
	void UpdateWalls();
	void UpdateAliens();
	void EraseInactiveEntities();
	void HandleLoseConditions();
	void Collision();
	void Render();

	void SpawnWalls();
	void SpawnAliens();

	bool CheckCollision(Vector2 circlePos, float circleRadius, Vector2 lineTop, Vector2 lineBottom);

	bool CheckNewHighScore();

	void InsertNewHighScore(std::string name);

	void SaveLeaderboard();


	// Entity Storage and Resources
	Resources resources;

	Player player;

	std::vector<Projectile> Projectiles; //TODO: pool?

	std::vector<Wall> Walls;

	std::vector<Alien> Aliens;

	std::vector<PlayerData> Leaderboard = { {"Player 1", 500}, {"Player 2", 400}, {"Player 3", 300}, {"Player 4", 200}, {"Player 5", 100} };

	Background background;



	Vector2 playerPos;
	Vector2 alienPos;
	Vector2 cornerPos;
	float offset;



	//TEXTBOX ENTER
	char name[9 + 1] = "\0";      //One extra space required for null terminator char '\0'
	int letterCount = 0;

	
	bool mouseOnText = false;

	int framesCounter = 0;

};