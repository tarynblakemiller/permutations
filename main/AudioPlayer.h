#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include <Audio.h>

class AudioPlayer {
public:
  AudioPlaySdWav* wav;
  AudioMixer4* mixer;
  AudioConnection* connection;
  bool isEmpty;
  int currentStemId;
  unsigned long startTime;
  float fadeInTime;
  float fadeOutTime;
  unsigned long startOffset;
  float volume;

  AudioPlayer(AudioPlaySdWav* w, AudioMixer4* m, AudioConnection* c);
  void playStem(const char* filename, int id, float fadeIn, float fadeOut, float vol, bool isLong);
  void updateFade();
};

extern AudioPlayer players[];

#endif