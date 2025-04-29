#ifndef STEMS_H
#define STEMS_H

struct Stem {
  const char* filename;
  int id;
  bool isLong;
  bool exclusive;
  float fadeIn;
  float fadeOut;
  float volume;
};

extern Stem stems[];

#endif
