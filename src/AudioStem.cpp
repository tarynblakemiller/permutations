#include "AudioStem.h"

AudioStem::AudioStem()
    : id(0),
      filename(""),
      groupId(0),
      player(nullptr),
      mixerLeft(nullptr),
      mixerRight(nullptr),
      channel(0),
      currentVolume(0.0f),
      fadeInDuration(0),
      fadeOutDuration(0),
      fadeInStartTime(0),
      fadeOutStartTime(0)
{
}

AudioStem::AudioStem(int id, const String& filename, AudioPlaySdWav* player, AudioMixer4* mixerL, AudioMixer4* mixerR, int chan, int groupId)
    : id(id),
      filename(filename),
      player(player),
      mixerLeft(mixerL),
      mixerRight(mixerR),
      channel(chan),
      groupId(groupId),
      currentVolume(1.0f),
      fadeInDuration(0),
      fadeOutDuration(0),
      fadeInStartTime(0),
      fadeOutStartTime(0)
{
    setVolume(1.0f); // Initialize to full volume
}

bool AudioStem::isPlaying() const {
    return player && player->isPlaying();
}

void AudioStem::play() {
    if (player) {
        player->play(filename.c_str());
    }
}

void AudioStem::stop() {
    if (player) {
        player->stop();
    }
    setVolume(0.0f);
}

void AudioStem::setVolume(float volume) {
    currentVolume = constrain(volume, 0.0f, 1.0f);
    if (mixerLeft && mixerRight) {
        mixerLeft->gain(channel, currentVolume);
        mixerRight->gain(channel, currentVolume);
    }
}

void AudioStem::fadeIn(unsigned long duration) {
    fadeInDuration = duration;
    fadeInStartTime = millis();
    fadeOutDuration = 0; // Cancel any active fade out

    // Start at zero volume
    setVolume(0.0f);

    // Start playback
    play();
}

void AudioStem::fadeOut(unsigned long duration) {
    // Only start fade out if currently playing
    if (isPlaying()) {
        fadeOutDuration = duration;
        fadeOutStartTime = millis();
        fadeInDuration = 0; // Cancel any active fade in
    }
}

void AudioStem::update() {
    unsigned long currentTime = millis();

    // Handle fade in
    if (fadeInDuration > 0) {
        unsigned long elapsed = currentTime - fadeInStartTime;

        if (elapsed < fadeInDuration) {
            float progress = (float)elapsed / fadeInDuration;
            setVolume(progress);
        } else {
            // Fade in complete
            setVolume(1.0f);
            fadeInDuration = 0;
        }
    }

    // Handle fade out
    if (fadeOutDuration > 0) {
        unsigned long elapsed = currentTime - fadeOutStartTime;

        if (elapsed < fadeOutDuration) {
            float progress = 1.0f - ((float)elapsed / fadeOutDuration);
            setVolume(progress);
        } else {
            // Fade out complete, stop playback
            stop();
            fadeOutDuration = 0;
        }
    }
}
