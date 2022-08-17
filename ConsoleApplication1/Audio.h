#pragma once
#include "SFML/Audio/Sound.hpp"
#include "SFML/Audio/SoundBuffer.hpp"

class Audio;

struct AudioClip {
	sf::Sound* sound = nullptr;
	sf::SoundBuffer* buffer = nullptr;
	float volume = 1.0f;
	float pitch = 1.0f;

	AudioClip() {}
	AudioClip(sf::SoundBuffer buf) {
		buffer = new sf::SoundBuffer(buf);
		sound = new sf::Sound();
		sound->setBuffer(*buffer);
	}


	void setVolume(float val) {
		volume = val;
		sound->setVolume(val);
	}

	void setPitch(int val) {
		pitch = val;
		sound->setPitch(val);
	}

	void setLoop(bool val) {
		sound->setLoop(val);
	}

	void play() {
		sound->play();
	}

	void setPlayingOffset(sf::Time t) {
		sound->setPlayingOffset(t);
	}
};


class Audio {
private:


	static Audio* instance;


public:
	float globalPitch = 1.0f;
	AudioClip shot;
	AudioClip them;
	AudioClip slow;
	AudioClip lHit;
	AudioClip hit;
	AudioClip power;

	Audio();
	static Audio* GetInstance();
	

	void Play(AudioClip s);

	void SetPitch(float val) {
		globalPitch = val;

		shot.setPitch(val);
		shot.pitch = val;

		them.setPitch(val);
		them.pitch = val;

		lHit.setPitch(val);
		lHit.pitch = val;

		power.setPitch(val);
		power.pitch = val;

		hit.setPitch(val);
		hit.pitch = val;
	}

};

