#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>
#include "AudioStem.h"

// Channel definitions
static constexpr auto LEFT_CHANNEL = 0U;
static constexpr auto RIGHT_CHANNEL = 1U;

// Audio objects
AudioPlaySdWav wav_player_0;
AudioPlaySdWav wav_player_1;
AudioOutputI2S i2s_output;
AudioMixer4 mixer_left;
AudioMixer4 mixer_right;

// Connections for first WAV player
AudioConnection patch_wav_player_0_mixer_L{wav_player_0, LEFT_CHANNEL, mixer_left, 0};
AudioConnection patch_wav_player_0_mixer_R{wav_player_0, RIGHT_CHANNEL, mixer_right, 0};

// Connections for second WAV player
AudioConnection patch_wav_player_1_mixer_L{wav_player_1, LEFT_CHANNEL, mixer_left, 1};
AudioConnection patch_wav_player_1_mixer_R{wav_player_1, RIGHT_CHANNEL, mixer_right, 1};

// Connections from mixer to output
AudioConnection patch_mixer_to_output_left{mixer_left, 0, i2s_output, LEFT_CHANNEL};
AudioConnection patch_mixer_to_output_right{mixer_right, 0, i2s_output, RIGHT_CHANNEL};

// Create AudioStem objects
AudioStem stem1(1, "SO1SLOWA.WAV", &wav_player_0, &mixer_left, &mixer_right, 0);
AudioStem stem2(2, "SO1MARBL.WAV", &wav_player_1, &mixer_left, &mixer_right, 1);

auto setup_audio() -> void {
    AudioMemory(20); // Increased for stability
    Serial.println("Audio memory allocated: 150 blocks");
}

auto setup_sd() -> void {
    if (!SD.begin(BUILTIN_SDCARD)) {
        Serial.println("SD card initialization failed!");
        while (1); // Halt if SD fails
    } else {
        Serial.println("SD card initialized successfully");
    }
}

auto setup() -> void {
    Serial.begin(9600);
    delay(1000); // Give Serial time to initialize
    Serial.println("Starting Permutations Test");
    setup_audio();
    setup_sd();
}

auto loop() -> void {
    static bool stem1_started = false;
    static bool stem2_started = false;

    // Update stems for fading
    stem1.update();
    stem2.update();

    // Debug CPU usage
    Serial.print("Audio CPU Usage: ");
    Serial.println(AudioProcessorUsage());
    Serial.println(AudioMemoryUsage());
    Serial.println(AudioMemoryUsageMax());

    // Play first WAV file
    if (!stem1.isPlaying() && !stem1_started) {
        Serial.println("Playing first WAV file");
        stem1.fadeIn(5000); // 5-second fade-in
        stem1_started = true;
    }

    // Play second WAV file after 3 seconds
    if (!stem2.isPlaying() && !stem2_started && millis() > 3000) {
        Serial.println("Playing second WAV file");
        stem2.fadeIn(5000); // 5-second fade-in
        stem2_started = true;
    }

    // Monitor playback status
    if (stem1.isPlaying() || stem2.isPlaying()) {
        Serial.println("At least one stem is playing");
    } else if (stem1_started || stem2_started) {
        Serial.println("Both stems finished, restarting in 3 seconds");
        delay(3000);
        stem1_started = false;
        stem2_started = false;
    }
}
