#include "Audio.h"
#include "Game.h"

Audio::Audio() {
	sf::SoundBuffer themBuf;
	themBuf.loadFromFile("res/them.wav");
	them = AudioClip(themBuf);
	them.setVolume(10.0f);
	them.setLoop(true);
	them.setPlayingOffset(sf::seconds(68.4f));

	them.play();
	sf::SoundBuffer stdShotBuf;
	stdShotBuf.loadFromFile("res/blipSelect.wav");
	shot = AudioClip(stdShotBuf);
	shot.setVolume(20.0f);

	sf::SoundBuffer lHitBuf;
	lHitBuf.loadFromFile("res/laserHit.wav");
	lHit = AudioClip(lHitBuf);
	lHit.setVolume(40.0f);

	sf::SoundBuffer slowBuf;
	slowBuf.loadFromFile("res/slow.wav");
	slow = AudioClip(slowBuf);

	sf::SoundBuffer hitBuf;
	hitBuf.loadFromFile("res/hitHurt.wav");
	hit = AudioClip(hitBuf);

	sf::SoundBuffer powerBuf;
	powerBuf.loadFromFile("res/powerUp.wav");
	power = AudioClip(powerBuf);
}

Audio* Audio::GetInstance() {
	if (!instance)
		instance = new Audio();
	return instance;
}

void Audio::Play(AudioClip s) {
	float pitch = (rand() % 3000) / 10000.0f;
	s.setPitch(globalPitch + pitch);
	s.play();
}

Audio* Audio::instance = nullptr;