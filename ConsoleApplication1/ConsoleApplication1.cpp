
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include "Tool.hpp"
#include <algorithm>
#include <vector>

#include <SFML/Graphics.hpp>
#include <SFML/Main.hpp>
#include <SFML/Window.hpp>
#include "Entity.h"
#include "World.h"
#include "Game.h"
#include "Audio.h"
#include "Command.h"

#include "imgui.h"
#include "imgui-SFML.h"

using namespace sf;

int main()
{
	RenderWindow window(VideoMode(1820, 950), "Astro Neon");
	window.setVerticalSyncEnabled(true);
	window.setFramerateLimit(60);
	World world(&window);

	Font font;
	font.loadFromFile("res/astron boy.ttf");
	Text text;
	text.setFont(font);
	text.setFillColor(sf::Color(200, 175, 0));
	text.setCharacterSize(50);
	world.scoretxt = &text;

	// Target cursor
	CircleShape cursor(20);
	cursor.setFillColor(Color::Red);
	cursor.setOrigin(20, 20);
	sf:Texture target;
	target.loadFromFile("res/target.png");
	cursor.setTexture(&target);
	window.setMouseCursorVisible(false);
	world.cursor = &cursor;

	Entity* lockTarget = nullptr;


#pragma region Bullet
	RectangleShape* bShape = new RectangleShape(Vector2f(35, 5));
	Laser b(bShape);
	world.dataPlay.push_back(&b);
#pragma endregion


#pragma region player	
	ConvexShape* pShape = new ConvexShape(11);
	pShape->setPoint(0, Vector2f(30, 0));						  /*		7 ----- 8				  */
	pShape->setPoint(1, Vector2f(13, 30));						  /*	  /				    	9	  */
	pShape->setPoint(2, Vector2f(60, 35));						  /*	 /				10			  */
	pShape->setPoint(3, Vector2f(18, 55));						  /*	6				  \			  */
	pShape->setPoint(4, Vector2f(-20, 30));						  /* 	|				   \		  */
	pShape->setPoint(5, Vector2f(-45, 20));						  /*	|		 X			0		  */
	pShape->setPoint(6, Vector2f(-45, -20));					  /*	5				  /			  */
	pShape->setPoint(7, Vector2f(-20, -30));					  /*	 \				 1			  */
	pShape->setPoint(8, Vector2f(18, -55));						  /*	  \						2	  */
	pShape->setPoint(9, Vector2f(60, -35));						  /*		4 ----- 3				  */
	pShape->setPoint(10, Vector2f(13, -30));			
	pShape->setFillColor(Color::Blue);
	pShape->setOutlineColor(Color::Yellow);
	pShape->setOutlineThickness(7);
	PlayerPad p(pShape, &b);
	p.setPosition(950, 400);
	world.dataPlay.push_back(&p);
#pragma endregion


	Game::create(&world, &p);
	Audio::GetInstance();
	Controls::GetInstance();

	ImGui::SFML::Init(window);

	double tStart = getTimeStamp();
	double tEnterFrame = getTimeStamp();
	double tExitFrame = getTimeStamp();
	double time = 0.0;

	int* thickness = new int(2);

	while (window.isOpen()) {
		double dt = tExitFrame - tEnterFrame;
		time += dt;
		tEnterFrame = getTimeStamp();
		sf::Event event;
		while (window.pollEvent(event)) {
			ImGui::SFML::ProcessEvent(event);
			if (event.type == sf::Event::Closed)
				window.close();
		}

		// CONTROLS
		Controls::GetInstance()->Update();	
		if (Controls::GetInstance()->lock) { // Can lock when using a controller
			if (!lockTarget) {
				// Find the nearest entity
				float mindist = FLT_MAX;
				for (auto e : world.dataPlay) {
					if (e->type == EType::Bullet || !e->visible) continue;
					float dist = Magnitude(e->getPosition() - cursor.getPosition());
					if (dist < mindist) {
						mindist = dist;
						lockTarget = e;
					}
				}
			}
			// Add an offet using the joystick
			sf::Vector2f offset = Controls::GetInstance()->aimingControl();
			offset.x *= 2;
			offset.y *= 2;
			cursor.setPosition(lockTarget->getPosition() + offset);
			cursor.setRotation(cursor.getRotation() + dt * 300);
			if (!lockTarget->visible) {
				lockTarget = nullptr;
				Controls::GetInstance()->lock = false;
			}
		}
		else {	// Free aiming
			if (lockTarget) lockTarget = nullptr;
			cursor.setPosition(Controls::GetInstance()->setCursor(&cursor));
		}

		// UPDATE
		if (Game::GetInstance()->GetGameState() == GameState::Playing)
		{
			p.getPosition();
			Vector2f aimDir = cursor.getPosition() - p.getPosition(); //control.aimingControl();
			float rotAngle = atan2(aimDir.y, aimDir.x) * RadToDeg();
			p.setRotation(rotAngle);

			// Shoot
			if (Controls::GetInstance()->shootControl()) {
				if (world.timeScale < 1.0f) {	// Normal shot
					if (p.shootflag) {
						b.create(p.getPosition().x, p.getPosition().y, aimDir.x, aimDir.y, world.timeScale * world.timeScale, 8);
						p.shootflag = false;
					}
				}
				else {	// Shot in slow motion
					b.create(p.getPosition().x, p.getPosition().y, aimDir.x, aimDir.y, 0.1f);
				}
			}
			else {
				p.shootflag = true;
			}

		}
		Game::GetInstance()->update(dt);


		// RENDERING
		Game::GetInstance()->draw(window);
		if (world.ImguiWindow) {
			ImGui::SFML::Update(window, sf::milliseconds((int)(dt * 1000.0)));
			(world.*(world.ImguiWindow))();
			ImGui::SFML::Render(window);
		}
		window.draw(cursor);
		window.display();
		tExitFrame = getTimeStamp();
	}
	ImGui::SFML::Shutdown();

	return 0;
}