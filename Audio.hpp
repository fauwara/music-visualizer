#pragma once

#include <iostream>
#include <SFML/Audio.hpp>

class Audio {

	public:
		sf::SoundBuffer buffer;
		sf::Sound song;
		unsigned int sampleRate;
		sf::Uint64 sampleCount;
		
		const sf::Int16* samples;
		sf::Uint64 sampleSize;
			
		// load raw audio data into buffer
		void loadBuffer( std::string audioFilePath) {

			if (!buffer.loadFromFile(audioFilePath)) {
				std::cout << "Couldn't load buffer" << std::endl;
			} else {
				song.setBuffer(buffer);
				sampleSize = buffer.getSampleCount();
				samples = buffer.getSamples();
			}

			sampleRate = buffer.getSampleRate();
			sampleCount = buffer.getSampleCount();

		}

};