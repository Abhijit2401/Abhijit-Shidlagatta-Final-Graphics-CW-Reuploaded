// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// Major logic written by me:
// - FMOD System initialisation and memory management
// - Discrete channels for ambient streams (Wind vs Ocean positional logic)
// - Dynamic engine audio (Pitch and volume mapped to vehicle speed)
// - Hit/Boost SFX triggers
// ---------------------------------------------------------
#include "Audio.h"
#include <math.h>
#include <iostream>

#pragma comment(lib, "lib/fmod_vc.lib")

CAudio::CAudio()
{
	// Sets everything to null first to prevent accidental use of uninitialised pointers
	m_eventSound = NULL;
	m_boostSound = NULL;
	m_music = NULL;
	m_musicChannel = NULL;
	m_ambientSound = NULL;
	m_ambientChannel = NULL;
	m_oceanSound = NULL;
	m_oceanChannel = NULL;
	m_engineSound = NULL;
	m_engineChannel = NULL;
}

CAudio::~CAudio()
{
	// Releases all the loaded fmod sounds before closing to prevent memory leaks
	if (m_eventSound)   m_eventSound->release();
	if (m_boostSound)   m_boostSound->release();
	if (m_music)        m_music->release();
	if (m_ambientSound) m_ambientSound->release();
	if (m_oceanSound)   m_oceanSound->release();
	if (m_engineSound)  m_engineSound->release();

	// Shuts down the core fmod system safely
	if (m_FmodSystem) {
		m_FmodSystem->close();
		m_FmodSystem->release();
	}
}

bool CAudio::Initialise()
{
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK) return false;

	// Initialises fmod with 32 virtual channels
	result = m_FmodSystem->init(32, FMOD_INIT_NORMAL, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK) return false;

	return true;
}

void CAudio::Update()
{
	m_FmodSystem->update();
}

void CAudio::FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char* errorString = FMOD_ErrorString(result);
		OutputDebugStringA("FMOD ERROR: ");
		OutputDebugStringA(errorString);
		OutputDebugStringA("\n");
	}
}

// SFX loading / playing
bool CAudio::LoadEventSound(const char* filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_eventSound);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::PlayEventSound()
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, NULL);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::LoadBoostSound(const char* filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_boostSound);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::PlayBoostSound()
{
	result = m_FmodSystem->playSound(m_boostSound, NULL, false, NULL);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

// Background music / ambient looping streams
bool CAudio::LoadMusicStream(const char* filename)
{
	result = m_FmodSystem->createStream(filename, FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::LoadAmbientSound(const char* filename)
{
	result = m_FmodSystem->createStream(filename, FMOD_LOOP_NORMAL, 0, &m_ambientSound);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::PlayAmbientSound()
{
	result = m_FmodSystem->playSound(m_ambientSound, NULL, false, &m_ambientChannel);
	if (m_ambientChannel) {
		m_ambientChannel->setVolume(1.0f);
	}
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

void CAudio::StopAmbientSound()
{
	if (m_ambientChannel) {
		m_ambientChannel->stop();
		m_ambientChannel = NULL;
	}
}

bool CAudio::LoadOceanSound(const char* filename)
{
	result = m_FmodSystem->createStream(filename, FMOD_LOOP_NORMAL, 0, &m_oceanSound);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::PlayOceanSound()
{
	result = m_FmodSystem->playSound(m_oceanSound, NULL, false, &m_oceanChannel);
	if (m_oceanChannel) {
		m_oceanChannel->setVolume(1.0f);
	}
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

void CAudio::StopOceanSound()
{
	if (m_oceanChannel) {
		m_oceanChannel->stop();
		m_oceanChannel = NULL;
	}
}

// Engine sound that reacts to speed
bool CAudio::LoadEngineSound(const char* filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_NORMAL, 0, &m_engineSound);
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

bool CAudio::PlayEngineSound()
{
	result = m_FmodSystem->playSound(m_engineSound, NULL, false, &m_engineChannel);
	if (m_engineChannel) {
		m_engineChannel->setVolume(0.3f);
	}
	FmodErrorCheck(result);
	return (result == FMOD_OK);
}

void CAudio::UpdateEnginePitch(float speed)
{
	if (m_engineChannel) {
		float absSpeed = fabs(speed);

		// Dynamically pitches up andn increases volume as the car accelerates
		float pitch = 1.0f + (absSpeed * 15.0f);
		m_engineChannel->setPitch(pitch);
		float volume = 0.02f + (absSpeed * 4.0f);

		// Caps volume
		if (volume > 0.35f) {
			volume = 0.35f;
		}

		m_engineChannel->setVolume(volume);
	}
}