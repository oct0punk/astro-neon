#pragma once
#include <chrono>
#include <ctime>
#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Image.hpp"
#include "SFML/Graphics/Texture.hpp"
#include "SFML/Graphics/RenderWindow.hpp"

using namespace std;
using namespace std::chrono;


inline double getTimeStamp() //retourne le temps actuel en seconde
{
	std::chrono::nanoseconds ns =
		duration_cast<std::chrono::nanoseconds>(system_clock::now().time_since_epoch());
	return ns.count() / 1000000000.0;
}


inline int Sign(float val) {
	if (val < 0)
		return -1;
	return 1;
}

inline float clamp(float val, float min, float max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

inline float lerp(float t, float min, float max) {
	return min + t * (max - min);
}

inline float catmull(float p0, float p1, float p2, float p3, float t) {
	auto q = 2.0f * p1;
	auto t2 = t * t;

	q += (-p0 + p2) * t;
	q += (2.0f * p0 - 5.0f * p1 + 4.0f * p2 - p3) * t2;
	q += (-p0 + 3 * p1 - 3 * p2 + p3) * t2 * t;

	return 0.5f * q;
}

inline double pi() {
	return 3.14159265359;
}

inline float RadToDeg() {
	return 57.2958f;
}

inline float DegToRad() {
	return 0.0174533f;
}


inline float Magnitude(sf::Vector2f v) {
	return sqrt(v.x * v.x + v.y * v.y);
}

inline void Normalize(sf::Vector2f* v) {
	float magnitude = sqrt(v->x * v->x + v->y * v->y);
	*v = sf::Vector2f(v->x / magnitude, v->y / magnitude);
}

inline float DotProduct(sf::Vector2f u, sf::Vector2f v) {
	float uNorm = sqrt(u.x * u.x + u.y * u.y);
	float vNorm = sqrt(v.x * v.x + v.y * v.y);
	return uNorm * vNorm * cos(atan2(0, u.x) - atan2(0, v.x));
}

inline sf::Vector2f Reflect(sf::Vector2f incident, sf::Vector2f normal) {
	// => For a given incident vector I and surface normal N reflect returns the reflection direction calculated as I - 2.0 * dot(N, I) * N.
	sf::Vector2f v;
	v.x = incident.x - 2.0f * DotProduct(normal, incident) * normal.x;
	v.y = incident.y - 2.0f * DotProduct(normal, incident) * normal.y;
	Normalize(&v);
	return v;
}

inline float Magnitude(float ux, float uy, float vx, float vy) {
	sf::Vector2f dir = sf::Vector2f(vx - ux, vy - uy);
	return sqrt(dir.x * dir.x + dir.y * dir.y);
}


// Screenshot to sf::Image
inline sf::Image Capture(sf::Window* w) {
	sf::Vector2u windowSize = w->getSize();
	sf::Texture texture;
	texture.create(windowSize.x, windowSize.y);
	texture.update(*w);
	return texture.copyToImage();
}