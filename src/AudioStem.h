#ifndef AUDIO_STEM_H
#define AUDIO_STEM_H

#include <Arduino.h>
#include <Audio.h>

class AudioStem {
public:
    AudioStem();
    AudioStem(int id, const String& filename, AudioPlaySdWav* player, AudioMixer4* mixerL, AudioMixer4* mixerR, int chan, int groupId = 0);

    // Getters
    int getId() const { return id; }
    const String& getFilename() const { return filename; }
    int getGroupId() const { return groupId; }
    bool isPlaying() const;
    float getCurrentVolume() const { return currentVolume; }

    // Control methods
    void play();
    void stop();
    void setVolume(float volume);
    void fadeIn(unsigned long duration);
    void fadeOut(unsigned long duration);
    void update();

private:
    int id;
    String filename;
    int groupId;
    float currentVolume;
    unsigned long fadeInDuration;
    unsigned long fadeOutDuration;
    unsigned long fadeInStartTime;
    unsigned long fadeOutStartTime;
    AudioPlaySdWav* player;
    AudioMixer4* mixerLeft;
    AudioMixer4* mixerRight;
    int channel;
};

#endif
