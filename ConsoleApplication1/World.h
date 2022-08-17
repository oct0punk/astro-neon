#pragma once
#include <vector>
#include "Entity.h"
#include "Tuto.h"
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/ConvexShape.hpp"
#include "imgui.h"
#include "imgui-SFML.h"


class World {
	bool aimDisplay = true;
	float camShake = 0.0f;
	float camShaketime = .2f;
	int camShakeIntensity = 20;

public:
	Button* selectedButton = nullptr;
	sf::RenderWindow* window = nullptr;
	sf::View view;
	Tuto* tuto;
	sf::CircleShape* cursor = nullptr;

	std::vector<Entity*>	dataPlay;
	std::vector<Particle*>	dataFX;
	std::vector<Button*>	dataMenu;
	std::vector<Button*>	dataGameOver;
	sf::Text title;
	sf::Text credit;
	sf::Text scoreEnd;
	sf::Text* scoretxt = nullptr;

	sf::Color* clearColor = new sf::Color(0, 0, 20, 0);
	void (World::* ImguiWindow)() = &World::Tools;

	float timeSinceLevelLoad = 0.0f;
	float timeScale = 1.0f;
	int eCount = 0;
	bool pauseKeyUp = true;
	bool lockWall = false;
	float flashTime = 0.0f;

	World(sf::RenderWindow* win) {
		Enemy::oColor = sf::Color::Blue;
		tuto = new Tuto();
		window = win;
		view.setCenter(win->getSize().x / 2, win->getSize().y / 2);
		view.setSize(win->getSize().x, win->getSize().y);

		// Used for buttons
		sf::Font* font = new sf::Font();
		font->loadFromFile("res/astro.ttf");
		sf::Font* astro = new sf::Font();
		astro->loadFromFile("res/astron boy wonder.ttf");
		

		sf::Text* text = new sf::Text("Play", *font);
		sf::RectangleShape* rect = new sf::RectangleShape(
			sf::Vector2f(
				text->getString().getSize() * text->getCharacterSize() * 1.32f,
				text->getCharacterSize() * 2.3f));
		rect->setOrigin(20, 20);
		rect->setOutlineThickness(14);
		rect->setOutlineColor(sf::Color::Yellow);


	#pragma region Button
		// --------------------------- PLAY BUTTON ---------------------------
		Button* menuPlay = new Button(rect, text, PlayMode);
		menuPlay->setPosition(500, 300);
		dataMenu.push_back(menuPlay);
		

		// --------------------------- OPTIONS BUTTON ---------------------------
		text->setString("Options");
		rect = new sf::RectangleShape(
			sf::Vector2f(
				text->getString().getSize() * text->getCharacterSize() * 1.24f,
				text->getCharacterSize() * 2.3f));
		rect->setOrigin(20, 20);
		Button* options = new Button(rect, text, OptionButton);
		options->setPosition(500, 500);
		dataMenu.push_back(options);

		// --------------------------- RETRY BUTTON ---------------------------
		text->setString("Retry");
		rect = new sf::RectangleShape(
			sf::Vector2f(
				text->getString().getSize() * text->getCharacterSize() * 1.24f,
				text->getCharacterSize() * 2.3f));
		rect->setOrigin(20, 20);
		Button* retry = new Button(rect, text, PlayMode);
		retry->setPosition(660, 666);
		dataGameOver.push_back(retry);

		// --------------------------- MAIN MENU BUTTON ---------------------------
		text->setString("Main Menu");
		rect = new sf::RectangleShape(
			sf::Vector2f(
				text->getString().getSize() * text->getCharacterSize() * 1.24f,
				text->getCharacterSize() * 2.3f));
		rect->setOrigin(20, 20);
		Button* menuButton = new Button(rect, text, BackToMenu);
		menuButton->setPosition(1260, 666);
		dataGameOver.push_back(menuButton);

		// --------------------------- QUIT GAME BUTTON ---------------------------
		text->setString("Quit");
		rect = new sf::RectangleShape(
			sf::Vector2f(
				text->getString().getSize() * text->getCharacterSize() * 1.24f,
				text->getCharacterSize() * 2.3f));
		rect->setOrigin(20, 20);
		rect->setOutlineThickness(-7);
		rect->setOutlineColor(sf::Color::Red);
		Button* quitButton = new Button(rect, text, QuitGame);
		quitButton->setPosition(500, 700);
		dataMenu.push_back(quitButton);
#pragma endregion


	#pragma region Texts

		// Print credit text written in "res/credit.txt"
		FILE* f = nullptr;
		fopen_s(&f, "res/credit.txt", "rb");
		char* str = nullptr;
		long size = 0;
		fseek(f, 0, SEEK_END);
		size = ftell(f);
		rewind(f);
		str = (char*)malloc(sizeof(char) * size);
		fread(str, 1, size, f);
		str[strlen(str) - 8] = 0;
		fclose(f);


		title.setFont(*astro);
		title.setString("ASTRO NEON");
		title.setCharacterSize(100);
		title.setPosition(150, 50);

		credit.setFont(*font);
		credit.setString(sf::String(str));
		credit.setCharacterSize(12);
		credit.setPosition(10, 10);
		
		scoreEnd.setFont(*astro);
		scoreEnd.setCharacterSize(100);
		scoreEnd.setPosition(200, 100);


#pragma endregion		

		// Stars in background
		for (int i = 0; i < 1000; i++) {
			sf::CircleShape shape(rand() % 100 / 100.0f);
			shape.setFillColor(sf::Color(255 - rand() % 25, 255 - rand() % 10, 255));
			shape.setPosition(rand() % window->getSize().x, rand() % window->getSize().y);
			dataFX.push_back(new Particle(EType::FX, new sf::CircleShape(shape)));
		}

	}



	void PushFX(Particle* p);
	void PushEntity(Entity* e, sf::Vector2f pos = sf::Vector2f(0, 0));
	void SpawnEnemy(sf::Vector2f pos = sf::Vector2f(0, 0));
	void SpawnObstacle(int radius, sf::Vector2f pos);
	void InstantiatePower();
	void KeepEntityOnScreen(Entity* e, float value = 50.0f);

	void updateGame(double dt);
	void updateMenu(double dt);
	void updatePause(double dt);
	void updateGameOver(double dt);

	void clear(sf::RenderWindow& win) { win.clear(*clearColor); }
	void drawGame(sf::RenderWindow& win);
	void drawMenu(sf::RenderWindow& win);
	void drawPause(sf::RenderWindow& win) {
		drawGame(win);
		for (auto e : dataGameOver) {
			e->draw(win);
		}
	}
	void drawGameOver(sf::RenderWindow& win);

	void Tools();
	void ColorsTool();
	void CamShake() {
		sf::Vector2f offset(rand() % camShakeIntensity - camShakeIntensity / 2, rand() % camShakeIntensity - camShakeIntensity / 2);
		sf::Vector2f center(window->getSize().x / 2, window->getSize().y / 2);
		view.setCenter(center + offset);
		window->setView(view);
	}
};
