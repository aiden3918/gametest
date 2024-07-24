#pragma once

#ifndef sound_h
#define sound_h

#include "../extern-lib/olcSoundWaveEngine.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>

struct SoundHandler {

	olc::sound::WaveEngine waveEngine;
	std::map<std::string, olc::sound::Wave*> sounds;
	std::vector<std::string> soundQueue;

	SoundHandler(const std::string soundFile) {
		waveEngine.InitialiseAudio();

		std::ifstream soundData(soundFile);
		if (!soundData.is_open()) {
			std::cout << "cannot open file " << soundFile << "; environment has not been created" << std::endl;
			return;
		}
		std::cout << "successfully opened file" << soundFile << std::endl;

		while (!soundData.eof()) {
			char fileline[500];
			std::stringstream stream;

			soundData.getline(fileline, 500);

			stream << fileline;
			
			std::string filename;
			std::string fileRef;
			stream >> filename >> fileRef;

			olc::sound::Wave soundWave;
			soundWave.LoadAudioWaveform(fileRef);
			std::cout << fileRef << " successfully loaded" << std::endl;

			sounds[filename] = &soundWave;
		}

		for (auto& [k, v] : sounds) {
			std::cout << k << ": " << v << std::endl;
		}

		waveEngine.SetOutputVolume(100.0f);

	}
	~SoundHandler() {}

	void addSoundToQueue(const std::string soundName) {
		soundQueue.push_back(soundName);
		std::cout << "added to queue: " << soundName << std::endl;
	}

	void update() {
		if (soundQueue.size() == 0) return;

		for (auto& s : soundQueue) {
			std::cout << "playing " << s << std::endl;
			waveEngine.PlayWaveform(sounds[s]);
		}

		soundQueue.clear();
	}
};

#endif