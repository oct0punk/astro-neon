#include "Command.h"

bool JoystickIsConnected() {
	return sf::Joystick::isConnected(0);
}


sf::Vector2f MoveJoystick() {
	sf::Vector2f dir(
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X),
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y)
	);

	if (Magnitude(dir) > 10.0f)
		Normalize(&dir);
	else
		return sf::Vector2f(.0f, .0f);
	return dir;
}

sf::Vector2f MoveMouse() {
	sf::Vector2f pPos;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
		pPos.y--;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
		pPos.y++;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
		pPos.x--;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		pPos.x++;
	}
	return pPos;
}


sf::Vector2f AimingJoystick() {
	return sf::Vector2f(
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U),
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V));
}

sf::Vector2f AimingMouse() {
	PlayerPad* p = Game::GetInstance()->player;
	sf::Vector2f aimDir = sf::Vector2f(
		sf::Mouse::getPosition(*Game::GetInstance()->world->window).x - p->getPosition().x,
		sf::Mouse::getPosition(*Game::GetInstance()->world->window).y - p->getPosition().y);
	Normalize(&aimDir);
	return aimDir;
}


bool ShootJoystick() {
	return sf::Joystick::isButtonPressed(0, 5);
}

bool ShootMouse() {
	return sf::Mouse::isButtonPressed(sf::Mouse::Left);
}

bool PowerJoystick() {
	return sf::Joystick::isButtonPressed(0, 8)
		&& sf::Joystick::isButtonPressed(0, 9);
}

bool PowerMouse() {
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Space);
}

bool SelectJoystick() { return sf::Joystick::isButtonPressed(0, 0); }

bool SelectMouse() { return sf::Mouse::isButtonPressed(sf::Mouse::Left); }

bool PauseJoystick() {
	return sf::Joystick::isButtonPressed(0, 7);
}

bool PauseMouse() {
	return sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
}


sf::Vector2f CursorJoystick(sf::CircleShape* cursor)
{
	sf::Vector2f dir(
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::U),
		sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::V)
	);
	dir.x *= Controls::GetInstance()->sensitivity;
	dir.y *= Controls::GetInstance()->sensitivity;
	dir += cursor->getPosition();
	dir.x = clamp(dir.x, 0, 1920 - cursor->getRadius());
	dir.y = clamp(dir.y, 0, 1080 - cursor->getRadius());
	return dir;
}

sf::Vector2f CursorMouse(sf::CircleShape* cursor) {
	sf::Vector2f cPos(
		sf::Mouse::getPosition(*Game::GetInstance()->world->window).x,
		sf::Mouse::getPosition(*Game::GetInstance()->world->window).y);
	return cPos;
}



Controls* Controls::instance;