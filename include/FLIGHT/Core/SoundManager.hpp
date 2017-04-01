#pragma once

#include <list>
#include <SFML/Audio.hpp>

namespace FLIGHT {
    class SoundManager {
	std::list<sf::Sound> m_runningSounds;
    public:
	void Play(sf::SoundBuffer & buffer);
	void Update();
    };
}
