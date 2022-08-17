#pragma once
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/Graphics/RenderWindow.hpp"


struct Controller {
	float joystickAngle = 0;

	sf::RectangleShape controller;
	sf::RectangleShape lJoystick;
	sf::RectangleShape rJoystick;
	sf::RectangleShape button0;
	sf::RectangleShape aimButton;
	sf::RectangleShape shootButton;

	
public:
	Controller();


	void setPosition(sf::Vector2f vec);

	void draw(sf::RenderWindow& win, sf::Vector2f pos, int animJoystick = 0);
};


class Tuto {
	Controller* controller = nullptr;
	sf::RectangleShape zqsd;
	sf::RectangleShape left;
	sf::RectangleShape space;
	sf::Color color = sf::Color::Red;	// highlight color
public:
	bool powerTuto = true;
	bool shootPowerTuto = true;

	Tuto();

	void DrawController(sf::RenderWindow& win, sf::Vector2f pos);

	void DrawButton0(sf::RenderWindow& win, sf::Vector2f pos);

	void DrawMoveCommand(sf::RenderWindow& win, sf::Vector2f pos);

	void DrawRJoystick(sf::RenderWindow& win, sf::Vector2f pos);

	void DrawPower(sf::RenderWindow& win, sf::Vector2f pos);

	void DrawAimButton(sf::RenderWindow& win, sf::Vector2f pos);

	void DrawShootButton(sf::RenderWindow& win, sf::Vector2f pos);
};