#include "AudioPlayer.h"
#include "Engine/Debug/LoggingManager.h"

AudioPlayer::AudioPlayer() :
	AudioEngine(nullptr)
{}

AudioPlayer::~AudioPlayer() {}

void AudioPlayer::InitAudioEngine()
{
	this->AudioEngine = irrklang::createIrrKlangDevice();
	if (!this->AudioEngine)
		LogManager::GetSingleton().OutputLog("An error occured while created IrrKlang audio device", LogLevel::FATAL);
}

void AudioPlayer::DestroyAudioEngine()
{
	this->AudioEngine->drop(); // Clean up audio engine
}

void AudioPlayer::SetAudioVolume(float Volume)
{
	this->AudioEngine->setSoundVolume(Volume);
}

void AudioPlayer::StopAllAudio()
{
	this->AudioEngine->stopAllSounds();
}

PlayableAudio AudioPlayer::PlayAudio(const char* Path, bool Loop, bool EnableTracking, bool SetPaused)
{
	return this->AudioEngine->play2D(Path, Loop, SetPaused, EnableTracking);
}

AudioPlayer& AudioPlayer::GetSingleton()
{
	static AudioPlayer Singleton;
	return Singleton;
}
