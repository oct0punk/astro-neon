#pragma once
#include "SFML/Window/Joystick.hpp"
#include "SFML/Graphics/RenderWindow.hpp"
#include "Tool.hpp"
#include "Game.h"
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

bool JoystickIsConnected();

sf::Vector2f MoveJoystick();
sf::Vector2f MoveMouse();

sf::Vector2f AimingJoystick();
sf::Vector2f AimingMouse();

bool ShootJoystick();
bool ShootMouse();

bool PowerJoystick();
bool PowerMouse();

bool SelectJoystick();
bool SelectMouse();

bool PauseJoystick();
bool PauseMouse();

sf::Vector2f CursorJoystick(sf::CircleShape* cursor);
sf::Vector2f CursorMouse(sf::CircleShape* cursor);


class Controls {
private:
	static Controls* instance;

public:
	static Controls* GetInstance() {
		if (!instance)
			instance = new Controls();
		return instance;
	}

	bool isConnected = false;
	float sensitivity = .5f;
	sf::Vector2f (*aimingControl)() = AimingMouse;
	sf::Vector2f (*moveControl)() = MoveMouse;
	bool (*shootControl)() = ShootMouse;
	bool (*powerControl)() = PowerMouse;
	bool (*pauseControl)() = PauseMouse;
	bool (*selectControl)() = SelectMouse;
	bool lockKeyDown = false;
	bool lock = false;
	sf::Vector2f (*setCursor)(sf::CircleShape*) = CursorMouse;

	void Update() {
		if (isConnected != JoystickIsConnected()) {	// Altern between Mouse Inputs and Controller Inputs
			isConnected = JoystickIsConnected();
			aimingControl = isConnected ?
				AimingJoystick :
				AimingMouse;
			shootControl = isConnected ?
				ShootJoystick :
				ShootMouse;
			moveControl = isConnected ?
				MoveJoystick :
				MoveMouse;
			setCursor = isConnected ?
				CursorJoystick :
				CursorMouse;
			powerControl = isConnected ?
				PowerJoystick :
				PowerMouse;
			selectControl = isConnected ?
				SelectJoystick :
				SelectMouse;
			pauseControl = isConnected ?
				PauseJoystick :
				PauseMouse;
		}

		if (isConnected) {	// Can lock an Entity when using a controller
			if (sf::Joystick::isButtonPressed(0, 4)) {
				if (!lockKeyDown) {
					lock = !lock;
					lockKeyDown = true;
				}
			} else {
				lockKeyDown = false;
			}
		}
	}

};