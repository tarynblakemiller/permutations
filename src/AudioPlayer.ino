#include "AudioPlayer.h"

AudioPlayer::AudioPlayer(AudioPlaySdWav* w, AudioMixer4* m, AudioConnection* c)
  : wav(w), mixer(m), connection(c), isEmpty(true), currentStemId(-1),
    startTime(0), fadeInTime(0.0), fadeOutTime(0.0), startOffset(0), volume(0.0) {
  mixer->gain(0, 0.0); // Start muted
}


  void AudioPlayer::playStem(const char* filename, int id, float fadeIn, float fadeOut, float vol, bool isLong) {
    wav->play(filename);
    currentStemId = id;
    isEmpty = false;
    startTime = millis();
    fadeInTime = fadeIn;
    fadeOutTime = fadeOut;
    volume = vol;
    startOffset = 0;
    if (isLong) {
      unsigned long length = wav->lengthMillis();
      if (length > 10000) {
        startOffset = random(length - 5000);
        // wav->positionMillis(startOffset);
      }
    }
    mixer->gain(0, volume);
  }

  void AudioPlayer::updateFade() {
    if (isEmpty) return;
    unsigned long elapsed = millis() - startTime;
    float duration = wav->lengthMillis();
    float gain = volume;

    if (fadeInTime > 0 && elapsed < fadeInTime * 1000) {
      float progress = (float)elapsed / (fadeInTime * 1000);
      mixer->gain(0, gain * progress);
      return;
    }
    if (fadeOutTime > 0 && elapsed > (duration - fadeOutTime * 1000)) {
      float progress = (float)(duration - elapsed) / (fadeOutTime * 1000);
      mixer->gain(0, gain * progress);
      return;
    }
    mixer->gain(0, gain);
  }


AudioPlayer players[] = {
  AudioPlayer(&playWav1, &mixer1, &patchCord1),
  AudioPlayer(&playWav2, &mixer2, &patchCord2),
  AudioPlayer(&playWav3, &mixer3, &patchCord3),
  AudioPlayer(&playWav4, &mixer4, &patchCord4),
  AudioPlayer(&playWav5, &mixer5, &patchCord5),
  AudioPlayer(&playWav6, &mixer6, &patchCord6),
};

const int numPlayers = sizeof(players) / sizeof(players[0]);
