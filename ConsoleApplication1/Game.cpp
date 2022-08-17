#include "Game.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/Text.hpp"

void Game::create(World* w, PlayerPad* p) {
	Instance = new Game(w);
	Instance->player = p;
}

Game* Game::GetInstance() {
	return Instance;
}

Game::Game(World* w) {
	world = w;

	FILE* f;
	fopen_s(&f, "res/register.txt", "rb");
	fscanf_s(f, "%i", &bestScore);
	fclose(f);
}

void Game::ChangeState(GameState nState) {
	state = nState;
	switch (state)
	{
	case Playing:
		world->ImguiWindow = nullptr;
		break;
	case Menu:
		world->ImguiWindow = &World::Tools;
		break;
	case Pause:
		break;
	case GameOver:
		if (score > bestScore) { // New record
			bestScore = score;
			world->scoreEnd.setFillColor(sf::Color::Yellow);
			world->scoreEnd.setString(sf::String("	You did : " + std::to_string(score) + "\nIt's a new record, congratulations!"));
			FILE* f;
			fopen_s(&f, "res/register.txt", "wb");
			fprintf(f, "");
			fprintf(f, std::to_string(score).c_str());
			fclose(f);
		}
		else {
			world->scoreEnd.setFillColor(sf::Color::Cyan);
			world->scoreEnd.setString(sf::String("You did : " + std::to_string(score) + "\nBest score was : " + std::to_string(bestScore)));
		}
		break;
	default:
		break;
	}
}

GameState Game::GetGameState() {
	return state;
}

void Game::update(double dt) {
	switch (state) {
	case Playing:
		world->updateGame(dt);
		if (enemyCount > 0) {
			if (time > 0) {
				time -= dt;
			}
			else {
				time = 0.3f + static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				world->SpawnEnemy(sf::Vector2f(rand() % 1000, -500 - rand() % 100));
				enemyCount--;
			}

		}
		else {
			if (world->eCount < 1)
				NextWave();
		}
		break;
	case Menu:
		world->updateMenu(dt);
		break;
	case Pause:
		world->updatePause(dt);
		break;
	case GameOver:
		world->updateGameOver(dt);
	default:
		break;
	}
}


void Game::draw(sf::RenderWindow& win) {
	switch (state)
	{
	case Playing:
		world->drawGame(win);
		break;
	case Menu:
		world->drawMenu(win);
		break;
	case Pause:
		world->drawPause(win);
		break;
	case GameOver:
		world->drawGameOver(win);
		break;
	default:
		break;
	}
}



void Game::NextWave() {
	wave++;
	enemyCount = wave * 3;
	int radius = 50 + rand() % 50;
	world->SpawnObstacle(radius, sf::Vector2f(rand() % 1920, rand() % 1080));
	radius = rand() % 100;
	world->SpawnObstacle(radius, sf::Vector2f(rand() % 1920, rand() % 1080));

	player->ChangeHealth(1);
	world->InstantiatePower();

}

int Game::EnemyHealth() { return -1 + wave * 2 + rand() % wave; }

// Launch a game from the beginning
void Game::Reset() {
	for (auto e : world->dataPlay) {	// Disable all elements in world
		if (e->type != Player || e->type != Bullet)
			e->visible = false;
		else
			e->visible = true;
	}
	world->eCount = 0;
	player->Revive();
	ChangeState(GameState::Playing);
	wave = 0;
	score = 0;
	NextWave();
}



Game* Game::Instance = nullptr;