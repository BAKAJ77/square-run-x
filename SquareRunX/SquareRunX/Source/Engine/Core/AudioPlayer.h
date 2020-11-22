#pragma once
#include <irrKlang.h>
#include <string>

typedef irrklang::ISound* PlayableAudio;

class AudioPlayer
{
	friend class EngineCore;
private:
	irrklang::ISoundEngine* AudioEngine;
private:
	AudioPlayer();
	~AudioPlayer();

	void InitAudioEngine();
	void DestroyAudioEngine();
public:
	void SetAudioVolume(float Volume);

	void StopAllAudio();
	PlayableAudio PlayAudio(const char* Path, bool Loop = false, bool EnableTracking = false, bool SetPaused = false);
public:
	static AudioPlayer& GetSingleton();
};