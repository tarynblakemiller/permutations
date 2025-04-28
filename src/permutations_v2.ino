#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "AudioStem.h"

//channel definitions
static constexpr auto LEFT_CHANNEL = 0U;
static constexpr auto RIGHT_CHANNEL = 1U;

AudioPlaySdWav sd_wav_player;
AudioOutputI2S i2s0;
//AudioMixer4 mixer1;
//AudioMixer4 mixer2;
AudioAmplifier amp_left;
AudioAmplifier amp_right;

////mixer
//AudioConnection patchCord1(sd_wav_player, 0, mixer1, 0);
//AudioConnection patchCord2(sd_wav_player, 0, mixer2, 1);
//
////I2S
//AudioConnection patchCord3(mixer1, 0, i2s0, 0);
//AudioConnection patchCord3(mixer2, 0, i2s0, 1);

AudioConnection patch_sd_to_amp_left{sd_wav_player, LEFT_CHANNEL, amp_left, 0};
AudioConnection patch_sd_to_amp_right{sd_wav_player, RIGHT_CHANNEL, amp_right, 0};
AudioConnection patch_amp_to_output_left{amp_left, 0, i2s0, LEFT_CHANNEL};
AudioConnection patch_amp_to_output_right{amp_right, 0, i2s0, RIGHT_CHANNEL};

// Create our AudioStem object
AudioStem stem1(1, "SO1SLOWA.WAV");

// Test sequence state
unsigned long lastStateTime = 0;
int testState = 0;


auto setup_audio() -> void
{
  AudioMemory(50);
//  mixer1.gain(0.9);
//  mixer2.gain(0.9);
  amp_left.gain(0.0);
  amp_right.gain(0.0);
}

auto setup_sd() -> void
{
  SD.begin(BUILTIN_SDCARD);
}

auto setup() -> void
{
  Serial.begin(9600);
  Serial.println("Starting Permutations Test");
  
  setup_audio();
  setup_sd();

  lastStateTime = millis(); 
 }


auto loop() -> void
{
   // Update stem to handle fading
  stem1.update();
  
  // Simple test sequence
  unsigned long currentTime = millis();

  
  switch (testState) {
    case 0:  // Start with fade in
      if (currentTime - lastStateTime > 2000) {
        Serial.println("Starting fade in (5 seconds)");
        stem1.fadeIn(5000);
        testState = 1;
        lastStateTime = currentTime;
      }
      break;
      
    case 1:  // Wait for a while at full volume
      if (currentTime - lastStateTime > 10000) {
        Serial.println("Starting fade out (3 seconds)");
        stem1.fadeOut(3000);
        testState = 2;
        lastStateTime = currentTime;
      }
      break;
      
    case 2:  // Wait until finished, then restart
      if (!stem1.isPlaying() && currentTime - lastStateTime > 5000) {
        Serial.println("Restarting test sequence");
        testState = 0;
        lastStateTime = currentTime;
      }
      break;
  
 }
}
