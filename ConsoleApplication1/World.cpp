#include "World.h"
#include"Tool.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "Game.h"
#include "Command.h"
#include "Audio.h"
#include <SFML/Window/Keyboard.hpp>

void World::updateGame(double dt) {
	bool coll = false;
	PlayerPad* p = Game::GetInstance()->player;
	p->update(dt);
	KeepEntityOnScreen(p);
	if (flashTime > 0.0f)
		flashTime -= dt * 14.0f;
	if (camShake > 0.0f) {
		camShake -= dt;
		CamShake();
		if (camShake <= 0.0f) {
			view.setCenter(window->getSize().x / 2, window->getSize().y / 2);
			window->setView(view);
		}
	}
	dt *= timeScale;

	for (auto e : dataPlay) {
		if (!e->visible || e->type == Player) continue;
		e->update(dt);

		switch (e->type) {
		case EType::Wall:
		{
			float distanceToP = Magnitude(p->getPosition().x, p->getPosition().y, e->getPosition().x, e->getPosition().y);
			float radius = e->spr->getGlobalBounds().width / 2;
			// The obstacle rebound on edges outside the screen
			sf::Vector2f pos = e->getPosition();
			if (e->getPosition().x < -radius * 3)
				pos.x = window->getSize().x + radius * 2;
			if (e->getPosition().x > window->getSize().x + radius * 3)
				pos.x = -radius * 2;
			if (e->getPosition().y < -radius * 3)
				pos.y = window->getSize().y + radius * 2;
			if (e->getPosition().y > window->getSize().y + radius * 3)
				pos.y = -radius * 2;
			e->setPosition(pos.x, pos.y);

			// Contact with player
			if (distanceToP < radius) {
				sf::Vector2f moveP(p->getPosition() - e->getPosition());
				moveP.x *= radius / distanceToP;
				moveP.y *= radius / distanceToP;
				p->setPosition(e->getPosition().x + moveP.x, e->getPosition().y + moveP.y);
			}
			// Contact with laser
			for (auto b : dataPlay) { // Rebound
				if (b->type == Bullet) {
					Laser* l = (Laser*)b;
					for (int i = 1; i <= l->px.size(); i++) { 
						sf::Vector2f bPos = sf::Vector2f(l->px[i - 1], l->py[i - 1]);
						float distance = Magnitude(e->getPosition().x, e->getPosition().y, bPos.x, bPos.y);
												
						// Collision
						if (distance < e->spr->getGlobalBounds().width / 2) {
							e->spr->setOutlineColor(sf::Color::Red);
							sf::Vector2f rebound = Reflect(sf::Vector2f(l->dx[i - 1], l->dy[i - 1]), bPos - e->getPosition());
							if (l->power[i - 1] > 200) {
								l->alive[i - 1] = false;
								break;
							}
							if (lockWall) {
								Entity* dirWall = nullptr;
								float dotMax = FLT_MIN;
								for (auto w : dataPlay) {
									if (w->type != Wall || e == w) continue;
									sf::Vector2f dir(w->getPosition() - sf::Vector2f(l->px[i - 1], l->py[i - 1]));	// Direction from laser to wall
									if (!dirWall) { dirWall = w; dotMax = DotProduct(rebound, dir); continue; }

									if (DotProduct(rebound, dir) > dotMax) {
										dotMax = DotProduct(rebound, dir);
										dirWall = w;
									}
								}

								if (dirWall && dotMax > .9f) {
									sf::Vector2f dir(dirWall->getPosition() - sf::Vector2f(l->px[i - 1], l->py[i - 1]));
									l->ChangeDirection(i - 1, dir.x, dir.y);
								}
								else
								{
									l->ChangeDirection(i - 1, rebound.x, rebound.y);	// Apply new default direction to bullet
								}
							}
							else {
								//l->px[i - 1] -= l->dx[i - 1] * 1.2f;		// Move back the laser
								//l->py[i - 1] -= l->dy[i - 1] * 1.2f;
								l->ChangeDirection(i - 1, rebound.x, rebound.y);	// Apply new default direction to bullet
							}
						}
					}
				}
			}
			break;
		}
		case EType::Bot:
		{
			Enemy* enemy = (Enemy*)e;
			// Collision with lasers and others ennemies
			for (auto b : dataPlay) {
				if (b->type == EType::Bullet) {
					Laser* l = (Laser*)b;
					for (int i = 0; i < l->px.size(); i++) {
						if (!l->alive[i]) continue;
						if (e->spr->getGlobalBounds().contains(sf::Vector2f(l->px[i], l->py[i]))) {	// Hit
							if (e->visible) {
								Game::GetInstance()->score += 1;
								if (enemy->ChangeHealth(-l->power[i])) {	// Kill
									eCount--;
									Game::GetInstance()->score += 10 * (l->power[i] / 1.9f);
									scoretxt->setString(to_string(Game::GetInstance()->score));
									scoretxt->setFillColor(sf::Color::Red);
									flashTime = pi();
									// FX
									sf::Color c = enemy->fColor;
									c.r += (rand() % 10) - 5;
									c.g += (rand() % 10) - 5;
									c.b += (rand() % 10) - 5;
									for (int i = 0; i < 40; i++) {
										sf::CircleShape* cShape = new sf::CircleShape(2 + rand() % 10);
										cShape->setFillColor(sf::Color(255 - rand() % 25, 255 - rand() % 50, 0));
										cShape->setOutlineColor(c);
										cShape->setOutlineThickness(1);
										Particle* p = new Particle(EType::Bot, cShape);
										p->dx =  cos(i * rand() % 300) * RadToDeg();
										p->dy =  sin(i * rand() % 300) * RadToDeg();
										p->setPosition(e->getPosition().x, e->getPosition().y);
										bool inserted = false;
										dataFX.push_back(p);
									}
								}
								if (l->power[i] > 1)	// CamShake
									camShake = camShaketime;
								l->alive[i] = false;
								Audio::GetInstance()->Play(Audio::GetInstance()->hit);
							}
						}
					}
				}
				if (b->type == EType::Bot) {
					sf::Vector2f dir = b->getPosition() - e->getPosition();
					if (Magnitude(dir) < 50) {	// Collide with particle									
						dir.x *= dt;
						dir.y *= dt;
						sf::Vector2f movePos(e->getPosition() - dir);
						e->setPosition(movePos.x, movePos.y);
					}
				}
			}
			
			//if (p->visible)
			//	KeepEntityOnScreen(e);

			// Contact with player
			if (Magnitude(e->getPosition() - p->getPosition()) < 150 && timeScale >= 1.0f)
				if (e->visible && p->visible) {
					p->ChangeHealth(-1);					
				}
			break;
		}
		case EType::Power:
			// Collision with lasers
			for (auto b : dataPlay) {
				if (b->type == EType::Bullet) {
					Laser* l = (Laser*)b;
					for (int i = 0; i < l->px.size(); i++) {
						if (!l->alive[i]) break;
						if (e->spr->getGlobalBounds().contains(sf::Vector2f(l->px[i], l->py[i]))) {
							if (e->ChangeHealth(-l->power[i])) {
								p->power++;
								Audio::GetInstance()->Play(Audio::GetInstance()->power);
							}
							l->alive[i] = false;
							Audio::GetInstance()->Play(Audio::GetInstance()->hit);

						}
					}
				}
			}
		}

		for (auto p : dataFX)
			p->update(dt);
	}

	if (Controls::GetInstance()->pauseControl()) {
		if (pauseKeyUp) {
			Game::GetInstance()->ChangeState(GameState::Pause);
			pauseKeyUp = false;
		}
	}
	else
		pauseKeyUp = true;

}


void World::updateMenu(double dt) {
	for (auto e : dataMenu) {
		e->update(dt);
		
		sf::Vector2f mPos(cursor->getPosition().x, cursor->getPosition().y);
		if (e->spr->getGlobalBounds().contains(mPos)) {
			selectedButton = e;

			if (Controls::GetInstance()->selectControl())
				e->state = ButtonState::Clicked;
			else
				e->state = ButtonState::Selected;		

		} else {
			if (selectedButton == e)
				selectedButton = nullptr;
			e->state = ButtonState::Normal;
		}
	}
}


void World::updatePause(double dt) {
	if (Controls::GetInstance()->pauseControl()) {
		if (pauseKeyUp) {
			Game::GetInstance()->ChangeState(GameState::Playing);
			pauseKeyUp = false;
		}
	}
	else
		pauseKeyUp = true;

	for (auto e : dataGameOver) {
		e->update(dt);

		sf::Vector2f mPos(cursor->getPosition().x, cursor->getPosition().y);
		if (e->spr->getGlobalBounds().contains(mPos)) {
			selectedButton = e;

			if (Controls::GetInstance()->selectControl())
				e->state = ButtonState::Clicked;
			else
				e->state = ButtonState::Selected;

		}
		else {
			if (selectedButton == e)
				selectedButton = nullptr;
			e->state = ButtonState::Normal;
		}
	}
}


void World::updateGameOver(double dt) {
	for (auto e : dataGameOver) {
		e->update(dt);

		sf::Vector2f mPos(cursor->getPosition().x, cursor->getPosition().y);
		if (e->spr->getGlobalBounds().contains(mPos)) {
			selectedButton = e;
			if (Controls::GetInstance()->selectControl())
				e->state = ButtonState::Clicked;
			else
				e->state = ButtonState::Selected;

		}
		else {
			if (selectedButton == e)
				selectedButton = nullptr;
			e->state = ButtonState::Normal;
		}
	}
}



void World::drawGame(sf::RenderWindow& window) {
	sf::Color clear = *clearColor;
	if (flashTime > 0.0f) {
		clear.r = lerp(sin(flashTime), clearColor->r, clearColor->r + 40);
		clear.g = lerp(sin(flashTime), clearColor->g, clearColor->g + 40);
		clear.b = lerp(sin(flashTime), clearColor->b, clearColor->b + 40);
	}
	window.clear(clear);
	for (auto p : dataFX)
		p->draw(window);
	for (auto e : dataPlay) {
		e->draw(window);
	}
	window.draw(*scoretxt);
	scoretxt->setFillColor(sf::Color(50, 50, 50));


	// Display Controls
	if (Game::GetInstance()->wave < 2) {
		tuto->DrawMoveCommand(window, sf::Vector2f(1700, 50));
		tuto->DrawShootButton(window, sf::Vector2f(1700, 300));
	}
	else if (Game::GetInstance()->wave < 3) {
		tuto->DrawRJoystick(window, sf::Vector2f(1600, 50));
		tuto->DrawAimButton(window, sf::Vector2f(1600, 50));
	}
	if (Game::GetInstance()->player->power > 0) {
		tuto->DrawPower(window, sf::Vector2f(1000, 200));
	}
	if (timeScale < 1.0f) {
		tuto->powerTuto = false;
		if (tuto->shootPowerTuto)
			tuto->DrawShootButton(window, sf::Vector2f(1000, 50));
	}
	else if (!tuto->powerTuto)
		tuto->shootPowerTuto = false;
}


void World::drawMenu(sf::RenderWindow& win) {
	win.clear(*clearColor);
	for (auto e : dataMenu) {
		e->draw(win);
	}
	win.draw(credit);
	win.draw(title);
	

	// Display controller's controls
	if (selectedButton) {
		tuto->DrawButton0(win, selectedButton->getPosition());
	}
	if (!aimDisplay) return;
	tuto->DrawRJoystick(*window, sf::Vector2f(1400, 600));
	if (Magnitude(cursor->getPosition()) > 800)
		aimDisplay = false;

}


void World::drawGameOver(sf::RenderWindow& win) {
	win.clear(*clearColor);
	for (auto e : dataGameOver) {
		e->draw(win);
	}
	win.draw(*scoretxt);
	win.draw(scoreEnd);

	// Display controller's controls
	if (selectedButton) {
		tuto->DrawButton0(*window, selectedButton->getPosition());
	}
}




void World::PushFX(Particle* p) {
	for (auto e : dataFX) {
		if ((!e->visible)) {
			e = new Particle(*p);
			return;
		}
	}
	dataFX.push_back(p);
}


void World::PushEntity(Entity* e, sf::Vector2f pos) {
	bool inserted = false;
	int idx = 0;
	for (auto entity : dataPlay) {
		if (entity->type == e->type) {
			if (!entity->visible) {
				inserted = true;
				entity->visible = true;
				entity->setPosition(pos.x, pos.y);
				break;
			}
		}
		idx++;
	}
	if (!inserted) dataPlay.push_back(e);
	e->setPosition(pos.x, pos.y);


	if (e->type != EType::Bot) return;
	eCount++;
	Enemy* enemy = (Enemy*)dataPlay[idx];
	enemy->SetHealth(Game::GetInstance()->EnemyHealth());
	for (auto b : dataPlay) {
		if (b->type == Player) {
			enemy->p = (PlayerPad*)b;
			break;
		}
	}
	
}


void World::SpawnEnemy(sf::Vector2f pos) {
	using namespace sf;
	ConvexShape* eShape = new ConvexShape(4);
	eShape->setPoint(0, Vector2f(0, 0));
	eShape->setPoint(1, Vector2f(80, 20));
	eShape->setPoint(2, Vector2f(0, 40));
	eShape->setPoint(3, Vector2f(20, 20));
	eShape->setOrigin(Vector2f(20, 20));
	eShape->setOutlineThickness(7);
	eShape->setFillColor(Enemy::fColor);
	eShape->setOutlineColor(Enemy::oColor);

	PushEntity(new Enemy(eShape), pos);

}


void World::SpawnObstacle(int radius, sf::Vector2f pos) {
	sf::CircleShape* wShape = new sf::CircleShape(radius);
	wShape->setOrigin(wShape->getRadius(), wShape->getRadius());
	wShape->setFillColor(sf::Color::Transparent);
	wShape->setOutlineThickness(-radius / 5);
	sf::Color oColor = rand() % 2 == 0 ? sf::Color(155, 100, 0) : sf::Color::Cyan;
	wShape->setOutlineColor(oColor);
	Entity* w = new Entity(EType::Wall, wShape);
	w->fillColor = sf::Color::Transparent;
	w->outlineColor = oColor;
	w->setPosition(pos.x, pos.y);
	dataPlay.push_back(w);
}


void World::InstantiatePower() {
	sf::CircleShape* cShape = new sf::CircleShape(20);
	cShape->setOrigin(20, 20);
	sf::Texture* watch = new sf::Texture();
	watch->loadFromFile("res/watch.png");
	cShape->setTexture(watch);
	cShape->setOutlineThickness(4);
	cShape->setOutlineColor(sf::Color::Yellow);
	Entity* power = new Entity(EType::Power, cShape);
	PushEntity(power);
	power->dx = -20;
	power->dy = 5;
	power->setPosition(1030, 250);
	power->SetHealth(20);
}


void World::KeepEntityOnScreen(Entity* e, float value) {
	// Keep the entity inside screen's bounds
	sf::Vector2f pPos = e->getPosition();
	pPos.x = clamp(pPos.x, value, window->getSize().x - value);
	pPos.y = clamp(pPos.y, value, window->getSize().y - value);
	e->setPosition(pPos.x, pPos.y);
}



void World::Tools() { using namespace ImGui;
ImGui::Begin("Gameplay");
ImGui::SliderFloat("sensitibility", &Controls::GetInstance()->sensitivity, 0.01f, 0.5f);
ImGui::Checkbox("Aim Assist", &lockWall);
ImGui::Separator();
ImGui::NewLine();
ImGui::InputInt("CamShake intensity", &camShakeIntensity, 1, 5);
ImGui::InputFloat("CamShake duration", &camShaketime, .01f, .1f);
ImGui::Separator();
ImGui::NewLine();
ImGui::InputFloat("Player Invincibility Duration", &Game::GetInstance()->player->invincibleTime, .01f, .1f);
ImGui::End();


#pragma region Audio

ImGui::Begin("Audio");
float themVol = Audio::GetInstance()->them.volume;
if (ImGui::SliderFloat("Music Volume", &themVol, 0.0f, 10.0f))
Audio::GetInstance()->them.setVolume(themVol);

float shotVol = Audio::GetInstance()->shot.volume;
if (ImGui::SliderFloat("Shot Volume", &shotVol, 0.0f, 50.0f))
Audio::GetInstance()->shot.setVolume(shotVol);

float hitVol = Audio::GetInstance()->hit.volume;
if (ImGui::SliderFloat("Enemy Hit Volume", &hitVol, 0.0f, 10.0f))
Audio::GetInstance()->hit.setVolume(hitVol);

float lHitVol = Audio::GetInstance()->lHit.volume;
if (ImGui::SliderFloat("Asteroid Hit Volume", &lHitVol, 0.0f, 10.0f))
Audio::GetInstance()->lHit.setVolume(lHitVol);

float powVol = Audio::GetInstance()->power.volume;
if (ImGui::SliderFloat("Got Power Volume", &powVol, 0.0f, 10.0f))
Audio::GetInstance()->power.setVolume(powVol);

float slowVol = Audio::GetInstance()->slow.volume;
if (ImGui::SliderFloat("Use Power Volume", &slowVol, 0.0f, 10.0f))
Audio::GetInstance()->slow.setVolume(slowVol);

ImGui::End();

#pragma endregion
}


void World::ColorsTool() {	using namespace ImGui;

	Begin("Background");
	float col[4]{ clearColor->r / 255.0f, clearColor->g / 255.0f, clearColor->b / 255.0f, clearColor->a / 255.0f };
	if (ImGui::ColorPicker4("Fill Color", col)) {
		clearColor->r = col[0] * 255.0f;
		clearColor->g = col[1] * 255.0f;
		clearColor->b = col[2] * 255.0f;
		clearColor->a = col[3] * 255.0f;
	}
	End();

	#pragma region Player
	
	Begin("Player Colors");
	PlayerPad* p = Game::GetInstance()->player;
	float pFillCol[4]{ p->fColor.r / 255.0f, p->fColor.g / 255.0f, p->fColor.b / 255.0f, p->fColor.a / 255.0f };
	if (ColorPicker4("Fill Color", pFillCol)) {
		p->fColor = sf::Color(
			pFillCol[0] * 255.0f,
			pFillCol[1] * 255.0f,
			pFillCol[2] * 255.0f,
			pFillCol[3] * 255.0f
		);
	}


	float pOutlineCol[4]{ p->oColor.r / 255.0f, p->oColor.g / 255.0f, p->oColor.b / 255.0f, p->oColor.a / 255.0f };
	if (ColorPicker4("Outline Color", pOutlineCol)) {
		p->oColor = sf::Color(
			pOutlineCol[0] * 255.0f,
			pOutlineCol[1] * 255.0f,
			pOutlineCol[2] * 255.0f,
			pOutlineCol[3] * 255.0f
		);
	}

	NewLine();
	Separator();
	NewLine();
	
	float pFillColHit[4]{ p->fColorInv.r / 255.0f, p->fColorInv.g / 255.0f, p->fColorInv.b / 255.0f, p->fColorInv.a / 255.0f };
	if (ColorPicker4("Hit Fill Color", pFillColHit)) {
		p->fColorInv = sf::Color(
			pFillColHit[0] * 255.0f,
			pFillColHit[1] * 255.0f,
			pFillColHit[2] * 255.0f,
			pFillColHit[3] * 255.0f
		);
	}

	float pOutlineColHit[4]{ p->oColorInv.r / 255.0f, p->oColorInv.g / 255.0f, p->oColorInv.b / 255.0f, p->oColorInv.a / 255.0f };
	if (ColorPicker4("Hit Outline Color", pOutlineColHit)) {
		p->oColorInv = sf::Color(
			pOutlineColHit[0] * 255.0f,
			pOutlineColHit[1] * 255.0f,
			pOutlineColHit[2] * 255.0f,
			pOutlineColHit[3] * 255.0f
		);
	}

	End();
	
	#pragma endregion
		
		
	#pragma region Enemy

	Begin("Enemmies Colors");
	float fCol[4]{ Enemy::fColor.r / 255.0f, Enemy::fColor.g / 255.0f, Enemy::fColor.b / 255.0f, Enemy::fColor.a / 255.0f };
	if (ColorPicker4("Fill Color", fCol)) {
		Enemy::fColor = sf::Color(
			fCol[0] * 255.0f,
			fCol[1] * 255.0f,
			fCol[2] * 255.0f,
			fCol[3] * 255.0f
		);
	}


	float oCol[4]{ Enemy::oColor.r / 255.0f, Enemy::oColor.g / 255.0f, Enemy::oColor.b / 255.0f, Enemy::oColor.a / 255.0f };
	if (ColorPicker4("Outline Color", oCol)) {
		Enemy::oColor = sf::Color(
			oCol[0] * 255.0f,
			oCol[1] * 255.0f,
			oCol[2] * 255.0f,
			oCol[3] * 255.0f
		);
	}

	NewLine();
	Separator();
	NewLine();

	float eFillColHit[4]{ Enemy::fColorHit.r / 255.0f, Enemy::fColorHit.g / 255.0f, Enemy::fColorHit.b / 255.0f, Enemy::fColorHit.a / 255.0f };
	if (ColorPicker4("Hit Fill Color", eFillColHit)) {
		Enemy::fColorHit = sf::Color(
			eFillColHit[0] * 255.0f,
			eFillColHit[1] * 255.0f,
			eFillColHit[2] * 255.0f,
			eFillColHit[3] * 255.0f
		);
	}

	float eOutlineColHit[4]{ Enemy::oColorHit.r / 255.0f, Enemy::oColorHit.g / 255.0f, Enemy::oColorHit.b / 255.0f, Enemy::oColorHit.a / 255.0f };
	if (ColorPicker4("Hit Outline Color", eOutlineColHit)) {
		Enemy::oColorHit = sf::Color(
			eOutlineColHit[0] * 255.0f,
			eOutlineColHit[1] * 255.0f,
			eOutlineColHit[2] * 255.0f,
			eOutlineColHit[3] * 255.0f
		);
	}

	End();

	#pragma endregion
}