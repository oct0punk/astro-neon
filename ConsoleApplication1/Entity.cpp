#include "Entity.h"
#include "Tool.hpp"
#include "Game.h"
#include "Audio.h"
#include "SFML/Window/Keyboard.hpp"
#include "Command.h"

void Entity::update(double dt) {
	timeSinceLevelStart += dt;
	if (!visible) return;
	if (cmds) applyCmdInterp(cmds, dt);

	sf::Vector2f move = getPosition();
	move.x += dx * dt;
	move.y += dy * dt;
	setPosition(move.x, move.y);

	if (type == EType::Wall)
		Movement(dt * 5);
}

void Entity::draw(sf::RenderWindow& win) {
	if (visible) {
		win.draw(*spr);
	}

	if (type == Wall) {
		if (spr->getOutlineColor() != outlineColor)
			spr->setOutlineColor(outlineColor);
	}
}

void Entity::Movement(double dt) {
	dx = sin(timeSinceLevelStart * dt) * 600 * dt;
	dy = cos(timeSinceLevelStart * .2f * dt) * 600 * dt;
}

bool Entity::ChangeHealth(int amount) {
	health += amount;
	if (health < 0) {
		visible = false;
		return true;
	}
	return false;
}


Cmd* Entity::applyCmdInterp(Cmd* cmd, double dt) {
	//dt = 1.0f / 60.0f * 0.1;
	bool destroy = false;
	switch (cmd->type) {
	case Move:
	{
		sf::Vector2f dir = sf::Vector2f(targetPos - getPosition());
		if (Magnitude(dir) < cmds->value * dt) {
			destroy = true;
			break;
		}
		Normalize(&dir);
		dir *= cmds->value * (float)dt;
		dir += getPosition();
		setPosition(dir.x, dir.y);
	}
	break;
	case Rotate:
		break;
	default:
		destroy = true;
		break;
	}


	if (!destroy) {
		return cmd;
	}
	else {
		cmd = cmd->popFirst();
		return cmd;
	}
}



PlayerPad::PlayerPad(sf::Shape* _spr, Laser* l) : Entity(EType::Player, _spr) {
	laser = l;
	PlayerPad::fColor = sf::Color(10, 100, 150);
	PlayerPad::oColor = sf::Color(230, 210, 85);
	PlayerPad::fColorInv = sf::Color(100, 0, 0);
	PlayerPad::oColorInv = sf::Color::Black;
}

void PlayerPad::update(double dt) {
	Entity::update(dt);

	if (invincible) {		
		spr->setFillColor(fColorInv);
		spr->setOutlineColor(oColorInv);

		invincibleTimer -= dt;
		if (invincibleTimer <= 0) {
			invincible = false;
			spr->setOutlineColor(oColor);
			spr->setFillColor(fColor);
		}
	}

	if (health < 1) {
		if (cos(timeSinceLevelStart * 10) > 0.0f) {
			if (spr->getFillColor().a > 0) {
				sf::Color transparent = spr->getFillColor();
				transparent.a = 0;
				spr->setFillColor(transparent);
				spr->setOutlineColor(sf::Color::Red);
			}
			else {
				if (spr->getFillColor().a < 255) {
					sf::Color opaque = spr->getFillColor();
					opaque.a = 255;
					spr->setFillColor(opaque);
					spr->setOutlineColor(oColor);
				}
			}
		}
	}
	else if (health < 3) {
		if (cos(timeSinceLevelStart * 20) < -.5f) {
			if (spr->getOutlineColor().a > 0) {
				sf::Color transparent = spr->getOutlineColor();
				transparent.a = 0;
				spr->setOutlineColor(transparent);
			}
			else {
				if (spr->getOutlineColor().a < 255) {
					sf::Color opaque = spr->getOutlineColor();
					opaque.a = 255;
					spr->setOutlineColor(opaque);
				}
			}
		}
	} 


	// Moves
	sf::Vector2f pPos = Controls::GetInstance()->moveControl();
	pPos.x *= speed * dt;
	pPos.y *= speed * dt;
	pPos += getPosition();
	setPosition(pPos.x, pPos.y);

	// Power
	if (Controls::GetInstance()->powerControl())
		if (power > 0 && powerTime <= .0f) {
			Power();
			powerTime = 4.2f;
			power--;
		}
	if (powerTime > 0.0f) {
		powerTime -= dt;
		if (powerTime <= 0.0f) {
			Game::GetInstance()->world->timeScale = 1.0f;
			Audio::GetInstance()->SetPitch(1.0f);
		}

	}

}

void PlayerPad::draw(sf::RenderWindow& win) {
	if (visible)
		win.draw(*spr);

}

bool PlayerPad::ChangeHealth(int amount) {
	if (invincible) return false;
	if (amount < 0) {
		invincible = true;
		invincibleTimer = invincibleTime;
		spr->setOutlineColor(oColorInv);
		spr->setFillColor(fColorInv);
	}
	else {
		spr->setOutlineColor(oColor);
		spr->setFillColor(fColor);
	}
	health += amount;
	if (health < 0) {
		visible = false;
		Game::GetInstance()->ChangeState(GameState::GameOver);
		return true;
	}
	return false;
}

void PlayerPad::Power() {
	Game::GetInstance()->world->timeScale = 0.01f;
	Audio::GetInstance()->SetPitch(0.3f);
	Audio::GetInstance()->slow.play();
	laser->reloading = 0.0f;
}




void Laser::create(float _px, float _py, float _dx, float _dy, float rTime, int p) {
	if (reloading > 0.0f) return;
	reloading = rTime;
	Audio::GetInstance()->Play(Audio::GetInstance()->shot);

	sf::Vector2f dir(_dx, _dy);
	Normalize(&dir);
	float dx = dir.x;
	float dy = dir.y;
	for (int i = 0; i < px.size(); ++i) {
		if (!alive[i]) {
			px[i] = _px;
			py[i] = _py;
			this->dx[i] = dx;
			this->dy[i] = dy;
			alive[i] = true;
			power[i] = p;
			return;
		}
	}
	px.push_back(_px);
	py.push_back(_py);
	this->dx.push_back(dx);
	this->dy.push_back(dy);
	alive.push_back(true);
	power.push_back(p);
}


void Laser::ChangeDirection(int idx, float x, float y) {
	sf::Vector2f dir(x, y);
	Normalize(&dir);
	dx[idx] = dir.x;
	dy[idx] = dir.y;
	power[idx] *= 4;
	Audio::GetInstance()->Play(Audio::GetInstance()->lHit);
}


void Laser::update(double dt) {
	if (reloading > 0.0f) {
		reloading -= dt;
	}
	for (int i = 0; i < px.size(); ++i) {
		if (alive[i]) {		// Move each shape into (dx ; dy)
			px[i] += dx[i] * speed * dt;
			py[i] += dy[i] * speed * dt;
			if (	// Check if outside screen
				(px[i] > 3000) || (px[i] < -100) ||
				(py[i] > 3000) || (py[i] < -100)
				) {
				alive[i] = false;
			}
		}
	}
}


void Laser::draw(sf::RenderWindow& win) {
	int idx = 0;
	const int sz = px.size();
	while (idx < sz) {
		if (alive[idx]) {
			auto b = spr;
			b->setRotation(atan2(dy[idx], dx[idx]) * 57.2958f);
			b->setPosition(sf::Vector2f(px[idx], py[idx]));
			win.draw(*b);
		}
		idx++;
	}
}




bool Enemy::ChangeHealth(int amount) {
	health += amount;
	hit = true;
	if (health < 0) {
		visible = false;
		spr->setFillColor(fColorHit);
		spr->setOutlineColor(oColorHit);
		return true;
	}
	return false;
}


void Enemy::update(double dt)
{
	if (!visible)
		return;
	Entity::update(dt);

	if (p) {
		if (p->visible) {
			// Look at player
			sf::Vector2f intoP = p->getPosition() - getPosition();
			float targetAngle = atan2(intoP.y, intoP.x) * RadToDeg();
			setRotation(targetAngle);

			// Move into player
			int speed = 300;
			Normalize(&intoP);
			dx = intoP.x * speed;
			dy = intoP.y * speed;
		}
		else
		{
			if (abs(dx + dy) > 0)
				SlowDown(200 * dt);
		}
	}
}


void Enemy::SlowDown(int speed) {
	dx = clamp(0.0f, dx - speed, dx + speed);
	dy = clamp(0.0f, dy - speed, dy + speed);
}


sf::Color Enemy::fColor = sf::Color::Blue;
sf::Color Enemy::oColor = sf::Color::White;
sf::Color Enemy::fColorHit = sf::Color::White;
sf::Color Enemy::oColorHit = sf::Color::White;



void PlayMode() { Game::GetInstance()->Reset(); }

void BackToMenu() {
	Game::GetInstance()->ChangeState(GameState::Menu);
}

void OptionButton() {

	Game::GetInstance()->world->ImguiWindow =
		Game::GetInstance()->world->ImguiWindow == &World::ColorsTool ?
		&World::Tools :
		&World::ColorsTool;
}

void QuitGame() {
	Game::GetInstance()->world->window->close();
}


Button::Button(sf::Shape* _spr, sf::Text* txt, void(*func)(void)) : Entity(EType::UI, _spr) {
	text = *txt;	// Set a text for the button
	action = func;	// Set a function to call when clicking on the button
}

void Button::update(double dt) {
	switch (state)
	{
	case Normal:
		spr->setFillColor(baseColor);
		break;
	case Selected:
		spr->setFillColor(selectedColor);
		break;
	case Clicked:
		if (!Controls::GetInstance()->selectControl())	// Use button function
			action();
		spr->setFillColor(clickedColor);
		break;
	default:
		spr->setFillColor(baseColor);
		break;
	}
}

void Button::draw(sf::RenderWindow& win) {
	Entity::draw(win);
	win.draw(text);
}




sf::Color Button::baseColor = sf::Color(sf::Color::Blue);
sf::Color Button::selectedColor = sf::Color(sf::Color::Cyan);
sf::Color Button::clickedColor = sf::Color(sf::Color(150, 50, 0));


void Particle::update(double dt) {
	if (!visible) return;
	Entity::update(dt);
	if (type == EType::Bot) {
		time -= dt;
		if (time > 0) {

			int r = lerp(time / originalTime, 0, color.r);
			int g = lerp(time / originalTime, 0, color.g);
			int b = lerp(time / originalTime, 0, color.b);
			int a = lerp(time / originalTime, 0, color.a);

			spr->setFillColor(sf::Color(r, g, b, a));
		}
		else
			visible = false;
	}
}
