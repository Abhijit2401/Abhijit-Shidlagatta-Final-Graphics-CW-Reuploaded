// ---------------------------------------------------------
// Editor: Abhijit Shidlagatta (210012420)
// Status: Uni template file, heavily modified with custom logic.
// ---------------------------------------------------------
#pragma once

#include <windows.h>
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"

class CAudio
{
public:
	CAudio();
	~CAudio();

	// basic fmod setup and tick
	bool Initialise();
	void Update();

	// one time sounds
	bool LoadEventSound(const char* filename);
	bool PlayEventSound();
	bool LoadBoostSound(const char* filename);
	bool PlayBoostSound();

	// ambient streaming 
	// uses streams for files like music
	bool LoadMusicStream(const char* filename);
	bool PlayMusicStream();

	bool LoadAmbientSound(const char* filename);
	bool PlayAmbientSound();
	void StopAmbientSound();

	bool LoadOceanSound(const char* filename);
	bool PlayOceanSound();
	void StopOceanSound();

	// dynamic sounds (loops continuously, pitch changes based on car speed)
	bool LoadEngineSound(const char* filename);
	bool PlayEngineSound();
	void UpdateEnginePitch(float speed);

private:
	// Helper to print fmod errors
	void FmodErrorCheck(FMOD_RESULT result);
	FMOD_RESULT result;
	FMOD::System* m_FmodSystem;

	// Sound buffers loaded into memory
	FMOD::Sound* m_eventSound;
	FMOD::Sound* m_boostSound;
	FMOD::Sound* m_engineSound;

	// audio streams read straight from the disk
	FMOD::Sound* m_music;
	FMOD::Sound* m_ambientSound;
	FMOD::Sound* m_oceanSound;
	FMOD::Channel* m_musicChannel;
	FMOD::Channel* m_ambientChannel;
	FMOD::Channel* m_oceanChannel;
	FMOD::Channel* m_engineChannel;
};