#pragma once
#include "World.h"

enum GameState {
	Playing,
	Menu,
	Pause,
	GameOver
};

class Game {
private:
	static Game* Instance;
	float time = 0.0f;
	GameState state = GameState::Menu;

public:
	PlayerPad* player = nullptr;
	World* world = nullptr;
	int score = 0;
	int bestScore = 0;
	int wave = 0;
	int enemyCount = 0;
	
	static void create(World* w, PlayerPad* p);
	static Game* GetInstance();
	Game(World* w);

	void ChangeState(GameState nState);
	GameState GetGameState();

	void update(double dt);
	void draw(sf::RenderWindow& win);
	void NextWave();
	int EnemyHealth();

	void Reset();
};

