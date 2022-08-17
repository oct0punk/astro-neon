#include "Tuto.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include "SFML/Window/Joystick.hpp"
#include "Tool.hpp"
#include "Command.h"


Controller::Controller() {
	sf::Texture conText;
	conText.loadFromFile("res/controller.png");
	conText.setSmooth(true);
	controller.setSize(sf::Vector2f(300, 200));
	controller.setTexture(new sf::Texture(conText));
	controller.setOrigin(400, 0);
	controller.setScale(.6f, .6f);

	sf::Texture lJoysText;
	lJoysText.loadFromFile("res/Ljoystick.png");
	lJoysText.setSmooth(true);
	lJoystick.setSize(sf::Vector2f(300, 200));
	lJoystick.setTexture(new sf::Texture(lJoysText));
	lJoystick.setOrigin(400, 0);
	lJoystick.setScale(.6f, .6f);
	lJoystick.setFillColor(sf::Color::Transparent);

	sf::Texture rJoysText;
	rJoysText.loadFromFile("res/Rjoystick.png");
	rJoysText.setSmooth(true);
	rJoystick.setSize(sf::Vector2f(300, 200));
	rJoystick.setTexture(new sf::Texture(rJoysText));
	rJoystick.setOrigin(400, 0);
	rJoystick.setScale(.6f, .6f);
	rJoystick.setFillColor(sf::Color::Transparent);

	sf::Texture butText;
	butText.loadFromFile("res/button0.png");
	butText.setSmooth(true);
	button0.setSize(sf::Vector2f(300, 200));
	button0.setTexture(new sf::Texture(butText));
	button0.setOrigin(400, 0);
	button0.setScale(.6f, .6f);

	sf::Texture aimButText;
	aimButText.loadFromFile("res/AimButton.png");
	aimButText.setSmooth(true);
	aimButton.setSize(sf::Vector2f(300, 200));
	aimButton.setTexture(new sf::Texture(aimButText));
	aimButton.setOrigin(400, 0);
	aimButton.setScale(.6f, .6f);
	aimButton.setFillColor(sf::Color::Transparent);

	sf::Texture shootButText;
	shootButText.loadFromFile("res/ShootButton.png");
	shootButText.setSmooth(true);
	shootButton.setSize(sf::Vector2f(300, 200));
	shootButton.setTexture(new sf::Texture(shootButText));
	shootButton.setOrigin(400, 0);
	shootButton.setScale(.6f, .6f);
	shootButton.setFillColor(sf::Color::Transparent);
}

void Controller::setPosition(sf::Vector2f vec)
{
	controller.setPosition(vec);
	lJoystick.setPosition(vec);
	rJoystick.setPosition(vec);
	button0.setPosition(vec);
	aimButton.setPosition(vec);
	shootButton.setPosition(vec);
}

void Controller::draw(sf::RenderWindow& win, sf::Vector2f pos, int animJoystick) {
	joystickAngle += .04f;


	setPosition(pos);

	if (animJoystick == 1) {
		sf::Vector2f lAngle(
			sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X),
			sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y));

		sf::Vector2f angleOffset =
			Magnitude(lAngle) > 10 ? lAngle :
			sf::Vector2f(cos(joystickAngle) * 100, sin(joystickAngle) * 100);

		angleOffset.x /= 8;
		angleOffset.y /= 8;
		lJoystick.setPosition(pos + angleOffset);

	}
	if (animJoystick == 2) {
		sf::Vector2f angleOffset =
			Magnitude(AimingJoystick()) > 10 ? AimingJoystick() :
			sf::Vector2f(cos(joystickAngle) * 100, sin(joystickAngle) * 100);

		angleOffset.x /= 8;
		angleOffset.y /= 8;
		rJoystick.setPosition(pos + angleOffset);
	}

	win.draw(controller);
	win.draw(lJoystick);
	win.draw(rJoystick);
	win.draw(button0);
	win.draw(aimButton);
	win.draw(shootButton);
}


void Tuto::DrawController(sf::RenderWindow& win, sf::Vector2f pos) {
	controller->controller.setFillColor(color);
	controller->draw(win, pos);
	controller->controller.setFillColor(sf::Color::White);
}

void Tuto::DrawButton0(sf::RenderWindow& win, sf::Vector2f pos) {
	if (!Controls::GetInstance()->isConnected) return;
	controller->button0.setFillColor(color);
	controller->draw(win, pos);
	controller->button0.setFillColor(sf::Color::White);
}

void Tuto::DrawMoveCommand(sf::RenderWindow& win, sf::Vector2f pos) {
	if (Controls::GetInstance()->isConnected) {
		controller->lJoystick.setFillColor(color);
		controller->draw(win, pos, 1);
		controller->lJoystick.setFillColor(sf::Color::Transparent);
		return;
	}
	zqsd.setPosition(pos);
	win.draw(zqsd);
}

void Tuto::DrawRJoystick(sf::RenderWindow& win, sf::Vector2f pos) {
	if (!Controls::GetInstance()->isConnected) return;
	controller->rJoystick.setFillColor(color);
	controller->draw(win, pos, 2);
	controller->rJoystick.setFillColor(sf::Color::Transparent);
}

void Tuto::DrawPower(sf::RenderWindow& win, sf::Vector2f pos) {
	if (!powerTuto) return;
	if (Controls::GetInstance()->isConnected) {
		controller->lJoystick.setFillColor(color);
		controller->draw(win, pos, 0);
		controller->lJoystick.setFillColor(sf::Color::Transparent);
		controller->rJoystick.setFillColor(color);
		controller->draw(win, pos, 0);
		controller->rJoystick.setFillColor(sf::Color::Transparent);
		return;
	}
	space.setPosition(pos);
	win.draw(space);
}

Tuto::Tuto() {

	controller = new Controller();

	sf::Texture zqsdText;
	zqsdText.loadFromFile("res/zqsd.png");
	zqsdText.setSmooth(true);
	zqsd.setSize(sf::Vector2f(300, 200));
	zqsd.setTexture(new sf::Texture(zqsdText));
	zqsd.setOrigin(400, 0);
	zqsd.setFillColor(sf::Color::Yellow);

	sf::Texture leftText;
	leftText.loadFromFile("res/left.png");
	leftText.setSmooth(true);
	left.setSize(sf::Vector2f(300, 200));
	left.setTexture(new sf::Texture(leftText));
	left.setOrigin(400, 0);
	left.setScale(.6f, .6f);
	left.setFillColor(sf::Color::Yellow);

	sf::Texture spaceText;
	spaceText.loadFromFile("res/space.png");
	spaceText.setSmooth(true);
	space.setSize(sf::Vector2f(300, 200));
	space.setTexture(new sf::Texture(spaceText));
	space.setOrigin(400, 0);
	space.setFillColor(sf::Color::Yellow);
}

void Tuto::DrawAimButton(sf::RenderWindow& win, sf::Vector2f pos) {
	if (!Controls::GetInstance()->isConnected) return;
	controller->aimButton.setFillColor(color);
	controller->draw(win, pos);
	controller->aimButton.setFillColor(sf::Color::White);
}

void Tuto::DrawShootButton(sf::RenderWindow& win, sf::Vector2f pos) {

	if (Controls::GetInstance()->isConnected) {
		controller->shootButton.setFillColor(color);
		controller->draw(win, pos);
		controller->shootButton.setFillColor(sf::Color::White);
		return;
	}
	left.setPosition(pos);
	win.draw(left);
}