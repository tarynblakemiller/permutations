#include <Audio.h>
#include <SD.h>
#include <SPI.h>
#include "AudioPlayer.h"
#include "Stems.h"


AudioPlaySdWav playWav1, playWav2, playWav3, playWav4, playWav5, playWav6;
AudioMixer4 mixer1, mixer2, mixer3, mixer4, mixer5, mixer6;
AudioMixer4 mainMixer1, mainMixer2;
AudioOutputI2S i2s_output;

AudioConnection patchCord1(playWav1, 0, mixer1, 0);
AudioConnection patchCord2(playWav2, 0, mixer2, 0);
AudioConnection patchCord3(playWav3, 0, mixer3, 0);
AudioConnection patchCord4(playWav4, 0, mixer4, 0);
AudioConnection patchCord5(playWav5, 0, mixer5, 0);
AudioConnection patchCord6(playWav6, 0, mixer6, 0);
AudioConnection patchCord9(mixer1, 0, mainMixer1, 0);
AudioConnection patchCord10(mixer2, 0, mainMixer1, 1);
AudioConnection patchCord11(mixer3, 0, mainMixer1, 2);
AudioConnection patchCord12(mixer4, 0, mainMixer1, 3);
AudioConnection patchCord13(mixer5, 0, mainMixer2, 0);
AudioConnection patchCord14(mixer6, 0, mainMixer2, 1);
AudioConnection patchCord17(mainMixer1, 0, i2s_output, 0);
AudioConnection patchCord18(mainMixer2, 0, i2s_output, 1);

//externals
extern AudioPlayer players[];
extern Stem stems[];
extern const int numPlayers;
extern const int numStems;

unsigned long sequenceStartTime = 0; 
int sequenceCount = 0; 

void setup() {
    AudioMemory(20);
    SPI.setMOSI(7);
    SPI.setSCK(14);
    if(!SD.begin(BUILTIN_SDCARD)) {
        while(true);
    }
  mainMixer1.gain(0, 1.0);
  mainMixer1.gain(1, 1.0);
  mainMixer1.gain(2, 1.0);
  mainMixer1.gain(3, 1.0);
  mainMixer2.gain(0, 1.0);
  mainMixer2.gain(1, 1.0);
  mainMixer2.gain(2, 1.0);
  mainMixer2.gain(3, 1.0);
  sequenceStartTime = millis();
}

void loop() {
  for (int i = 0; i < numPlayers; i++) {
    if (players[i].isEmpty) {
      assignNewStem(i);
    } else {
      players[i].updateFade();
    }
    if (!players[i].isEmpty && !players[i].wav->isPlaying()) {
      players[i].mixer->gain(0, 0.0);
      players[i].isEmpty = true;
      players[i].currentStemId = -1;
    }
  }
  delay(10);
}

void assignNewStem(int playerIndex) {
  for (int i = 0; i < numPlayers; i++) {
    if (!players[i].isEmpty && players[i].currentStemId != -1) {
      int stemIndex = findStemIndex(players[i].currentStemId);
      if (stemIndex != -1 && stems[stemIndex].exclusive) {
        unsigned long elapsed = millis() - players[i].startTime;
        float duration = players[i].wav->lengthMillis();
        if (elapsed < (duration - players[i].fadeOutTime * 1000)) {
          return;
        }
      }
    }
  }

  int availableStems[numStems];
  int availableCount = 0;
  for (int i = 0; i < numStems; i++) {
    bool isPlaying = false;
    for (int j = 0; j < numPlayers; j++) {
      if (players[j].currentStemId == stems[i].id) {
        isPlaying = true;
        break;
      }
    }
    if (!isPlaying) {
      availableStems[availableCount++] = i;
    }
  }

  if (availableCount == 0) {
    if (++sequenceCount >= numStems) {
      sequenceStartTime = millis();
      sequenceCount = 0;
    }
    return;
  }

  int stemIndex = availableStems[random(availableCount)];
  Stem& stem = stems[stemIndex];
  // unsigned long offset = (millis() - sequenceStartTime) % 10000;
  players[playerIndex].playStem(stem.filename, stem.id, stem.fadeIn, stem.fadeOut, stem.volume, stem.isLong);
}

int findStemIndex(int id) {
  for (int i = 0; i < numStems; i++) {
    if (stems[i].id == id) return i;
  }
  return -1;
}
