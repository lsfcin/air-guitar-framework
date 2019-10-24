#ifndef __SOUND_MODULE_H__
#define __SOUND_MODULE_H__

#include <vector>
#include <windows.h>
#include <math.h>
#include <fstream>
#include "win32timer.h"

#include "FMOD/fmod.hpp"
#include "FMOD/fmod_errors.h"

#define DISTINCT_NOTES 12 //tempered-scale notes (C to B), if this value changes the semi_tone_factor must be updated
#define SEMI_TONE_FACTOR 1.059f + (0.045f / 99) //multiplier to reach the next semi tone in tempered-scales

//effect mode
#define NO_EFFECT 0
#define FRET_SLIDE 1
#define FREE_SLIDE 2
#define VIBRATO 3

//frequency increment
#define JUST_INTONATION 0
#define EQUAL_TEMPERED 1

//playback command
#define PLAY_PAUSE 0
#define RESET 1
#define VOLUME_UP 2
#define VOLUME_DOWN 3

using namespace std;

typedef struct Pick {
	float intensity;
	float distance;
} Pick;

class SoundModule {
public:
	SoundModule(char* sounds_url, int nFrets, int minimumIntensity);
	~SoundModule() {}

	void mapFrequency(float distance, int effect_mode, int freq_inc_mode);
	void mapVolume(int time);
	void playPick(Pick pick, int freq_inc_mode = 0);
	
  void updateBaseFrequency();
	void initalizeFMOD(char* sounds_url);
	void checkErrorFMOD(FMOD_RESULT result);
	void playFMOD();
	void playStopChannelFMOD();
  void managePlayback(int command);

  float getFrequencyByPitch(int pitch, int freq_inc_mode);
  string getNote(float distance);
  int getSoundIndex() {
    return this->soundIndex;
  }
  int getEffectMode() {
    return this->effect_mode;
  }
  float getPlaybackVolume() {
    playback_channel->getVolume( &playback_volume );
    return playback_volume;
  }
  float getPlaybackPlaying() {
    bool playing;
    playback_channel->isPlaying( &playing );
    return playing;
  }
  float getPlaybackPaused() {
    bool paused;
    playback_channel->getPaused( &paused );
    return paused;
  }
  void setSoundIndex(int newIndex) {
    soundIndex = newIndex % nSounds;
    this->updateBaseFrequency();
  }
  void setEffectMode(int effect_mode) {
    this->effect_mode = effect_mode % 4;
  }
  void setToneMapPosition(int position) {
    if(toneMap[position]) {
      toneMap[position] = false;
      --toneSize;
    }
    else {
      toneMap[position] = true;
      ++toneSize;
    }
  }
  bool getToneMapPosition(int position) {
    return this->toneMap[position];
  }
  void setNFrets(int nFrets) {
    this->nFrets = nFrets;
  }

  void setMinimunIntensity(int minimumIntensity) {
    this->minimumIntensity = minimumIntensity;
  }

private:
  int nFrets;           //number of frets
  int baseFrequency;    //frequency of the main sound
  unsigned int envelopeDuration; //milliseconds
  int minimumIntensity; //minimum intensity to play sound

	float frequency;
	float volume;
  float playback_volume;
	float intensity;
	bool playing;
	bool picking;
	win32timer playingTimer;

  bool toneMap[DISTINCT_NOTES];
  int toneSize;
	vector<float> envelope;

  int effect_mode;

	//FMOD code
	int soundIndex;
  int nSounds;
	FMOD::System *system;
  FMOD::Channel *guitar_channel;
	FMOD::Channel *playback_channel;
	FMOD::Sound *playback_sound;
	vector<FMOD::Sound *> sounds;
  FMOD_RESULT result;
	unsigned int version;
	bool channelPlaying;
};

#endif //__SOUND_MODULE_H__