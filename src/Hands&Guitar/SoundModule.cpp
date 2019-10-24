#include "SoundModule.h"

SoundModule::SoundModule(char* sounds_url, int nFrets, int minimumIntensity) 
                       : nFrets(nFrets), minimumIntensity(minimumIntensity) {

	playingTimer.reset();

	envelope.push_back(1.0);
	envelope.push_back(1.0);
	envelope.push_back(1.0);
	envelope.push_back(0.8);
	envelope.push_back(0.6);
	envelope.push_back(0.4);
	envelope.push_back(0.3);
  envelope.push_back(0.2);
	envelope.push_back(0.1);

  for(int i = 0; i < DISTINCT_NOTES; ++i) toneMap[i] = true;
  toneSize = DISTINCT_NOTES;

	playing = false;
	picking = false;

  this->frequency = 0;
  this->effect_mode = 0;

	initalizeFMOD(sounds_url);
}

string SoundModule::getNote(float distance) {

  int tempF = frequency / 100;
  char resultC[20];
  itoa(tempF, resultC, 10);

  return resultC;
}

float SoundModule::getFrequencyByPitch(int pitch, int freq_inc_mode) {
  int octave = pitch / DISTINCT_NOTES;
  int note = pitch % DISTINCT_NOTES;

  int result = this->baseFrequency;

  if( freq_inc_mode == 0 ) {
    switch(note) {
      //case 0 is not needed cause no alteration is done
      case 1:
        result *= 1.067f;
        break;
      case 2:
        result *= 1.125f;
        break;
      case 3:
        result *= 1.200f;
        break;
      case 4:
        result *= 1.250f;
        break;
      case 5:
        result *= 1.333f;
        break;
      case 6:
        result *= 1.400f;
        break;
      case 7:
        result *= 1.500f;
        break;
      case 8:
        result *= 1.600f;
        break;
      case 9:
        result *= 1.667f;
        break;
      case 10:
        result *= 1.800f;
        break;
      case 11:
        result *= 1.875f;
        break;
      default:
        break;
    }
  }

  else if( freq_inc_mode == 1 ) {
    for(int i = 0; i < note; ++i) {
      result *= SEMI_TONE_FACTOR;
    }
  }

  result * pow((double)2, octave);
  return result;
}

void SoundModule::mapFrequency(float distance, int effect_mode, int freq_inc_mode) {
	
  float inverseDistance = 1 - distance;
	int fret = inverseDistance * this->nFrets;

	int octave = fret / toneSize;
  int posTone = fret % toneSize;
  int count = 0;
  int note = 0;
  for(int i = 0; i < DISTINCT_NOTES; ++i) {
    if(toneMap[i]) {
      if(count == posTone) {
        note = i;
        break;
      }
      ++count;
    }
  }

  int pitch = octave * DISTINCT_NOTES + note;

  frequency = getFrequencyByPitch(pitch, freq_inc_mode);

	if(octave > 0) frequency *= 2 * octave;

  if( effect_mode == 2 ) { //free slide mode
    //calculating next fret frequency
    ++fret;
    octave = fret / toneSize;
    posTone = fret % toneSize;
    count = 0;
    note = 0;
    for(int i = 0; i < DISTINCT_NOTES; ++i) {
      if(toneMap[i]) {
        if(count == posTone) {
          note = i;
          break;
        }
        ++count;
      }
    }
    pitch = octave * DISTINCT_NOTES + note;
    float nextFrequency = getFrequencyByPitch(pitch, freq_inc_mode);

    //interpolating frequencies
    float remainingDistance = inverseDistance * this->nFrets - fret;
    frequency = (1-remainingDistance) * frequency + (remainingDistance) * nextFrequency;
  }

  else if( effect_mode == 3 ) {//vibrato mode
    float midFretDistance = inverseDistance * this->nFrets - fret - 0.5f;
    frequency += midFretDistance * (frequency * (SEMI_TONE_FACTOR - 1));
  }
}

void SoundModule::mapVolume(int time) {
	float envlopePosition = (float) (time * (envelope.size() - 1)) / this->envelopeDuration;
	int first = envlopePosition;

	float precisionDistance = envlopePosition - (float)first;

	if(precisionDistance == 0) {
		volume = envelope.at(first) * intensity;
	} else {
		volume = envelope.at(first) * (1 - precisionDistance) + 
				     envelope.at(first + 1) * (precisionDistance);
	}
	volume *= intensity;
}

//effect_mode == 0 (without slide)
//effect_mode == 1 (fret-based slide)
//effect_mode == 2 (free slide)
//effect_mode == 3 (vibrato slide)
//freq_inc_mode 0 (just intonation)
//freq_inc_mode 1 (equal tempered)
void SoundModule::playPick(Pick pick, int freq_inc_mode) {
	//for fret-slide and free-slide modes the frequency is always mapped
  if( effect_mode == FRET_SLIDE || effect_mode == FREE_SLIDE )
    mapFrequency(pick.distance, effect_mode, freq_inc_mode);

  //for vibrato mode the frequency is mapped just if the current fret is not changed (lastFret == fret)
  //or if the player just had the intentio to play (pick.intensity > this->minimumIntensity)
  if( effect_mode == VIBRATO ) {
    static int lastFret = 0;

    float inverseDistance = 1 - pick.distance;
	  int fret = inverseDistance * this->nFrets;

    if(lastFret == fret || pick.intensity > this->minimumIntensity) {
      mapFrequency(pick.distance, VIBRATO, freq_inc_mode);
      lastFret = fret;
    }
  }

  if(pick.intensity > this->minimumIntensity) {
		playingTimer.reset();
		intensity = log(pick.intensity / 5) / 2; //TODO achar a função ideal
		picking = true;
		playing = true;
    if( effect_mode == NO_EFFECT ) 
      mapFrequency(pick.distance, NO_EFFECT, freq_inc_mode);
	} else {
		picking = false;
	}

	unsigned int time = playingTimer.getMilliseconds();

  if(time > this->envelopeDuration) {
		playing = false;
		volume = 0;
	} else {
		mapVolume(time);
	}

	playFMOD();
}



//----------------------------------------------FMOD---------------------------------------------//

void SoundModule::updateBaseFrequency() {
  float* trash1 = 0;
  int* trash2 = 0;
  float* currentFrequency = (float*)malloc(sizeof(float));
  sounds.at(this->soundIndex)->getDefaults(currentFrequency, trash1, trash1, trash2);
  this->baseFrequency = currentFrequency[0];

  unsigned int* length = (unsigned int*) malloc(sizeof(unsigned int));
  this->sounds.at(this->soundIndex)->getLength(length, FMOD_TIMEUNIT_MS);
  this->envelopeDuration = length[0];
}

void SoundModule::initalizeFMOD(char* sounds_url) {
	channelPlaying = false;

	guitar_channel = 0;
	playback_channel = 0;

	result = FMOD::Memory_Initialize(malloc(10 * 1024*1024), 10 * 1024*1024, 0, 0, 0);
  checkErrorFMOD(result);

  result = FMOD::System_Create(&system);
  checkErrorFMOD(result);

  result = system->getVersion(&version);
  checkErrorFMOD(result);

  if (version < FMOD_VERSION)
  {
      printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
      exit(0);
  }

	result = system->setOutput(FMOD_OUTPUTTYPE_DSOUND);
	checkErrorFMOD(result);
	result = system->setSoftwareFormat(44100, FMOD_SOUND_FORMAT_PCM16, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
  checkErrorFMOD(result);

  result = system->init(32, FMOD_INIT_NORMAL, NULL);
  checkErrorFMOD(result);

  ifstream soundsFile(sounds_url);
  char temp[200];
  soundsFile >> temp;
  soundsFile >> nSounds;
  if(nSounds > 0) {
    sounds.resize(nSounds);
    char soundUrl[200];
    for(int i = 0; i < nSounds; ++i) {
      soundsFile >> soundUrl;
      result = system->createStream(soundUrl, FMOD_2D | FMOD_SOFTWARE, 0, &(sounds.at(i)));
    }
    this->soundIndex = 0;
    this->updateBaseFrequency();

  } else {
    printf("O arquivo [files/guitar/sounds/sounds.txt] deve ter o endereço de pelo menos um arquivo de som!");
    exit(0);
  }

  //setting the playback sound
  soundsFile >> temp;
  char playbackUrl[200];
  soundsFile >> playbackUrl;
  result = system->createStream( playbackUrl, FMOD_2D | FMOD_SOFTWARE, 0, &playback_sound );
}

void SoundModule::playFMOD() {
	if(playing) {
    guitar_channel->setFrequency(frequency);
    if(!channelPlaying) {      
			channelPlaying = true;
		}

		if(picking) {
      playStopChannelFMOD();			
		}
	} else {
		if(channelPlaying) {
			channelPlaying = false;
		}
	}

	guitar_channel->setVolume(volume);
	system->update();
}

void SoundModule::playStopChannelFMOD() {
	result = system->playSound( FMOD_CHANNEL_FREE, sounds.at(soundIndex), false, &guitar_channel );
}

void SoundModule::managePlayback(int command) {

  if( command == RESET ) {
    playback_channel->getVolume( &playback_volume );
    playback_channel->setPaused( false );
    system->playSound( FMOD_CHANNEL_FREE, playback_sound, false, &playback_channel );
  }

  else {
    if( command == PLAY_PAUSE ) {
      bool playing;
      playback_channel->isPlaying( &playing );
      if( !playing ) {
        playback_channel->getVolume( &playback_volume );
        system->playSound( FMOD_CHANNEL_FREE, playback_sound, false, &playback_channel );
      }
      else {
        bool paused;
        playback_channel->getPaused( &paused );
        paused = !paused;
        playback_channel->setPaused( paused );
      }
    }
  }

  if( command == VOLUME_UP ) {
    playback_channel->getVolume( &playback_volume );
    playback_volume = playback_volume + 0.1f;
    if(playback_volume > 1) playback_volume = 1;
    playback_channel->setVolume( playback_volume );
  }
  else if( command == VOLUME_DOWN ) {
    playback_channel->getVolume( &playback_volume );
    playback_volume = playback_volume - 0.1f;
    if(playback_volume < 0) playback_volume = 0;
    playback_channel->setVolume( playback_volume );
  }
}

void SoundModule::checkErrorFMOD(FMOD_RESULT result) {
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}