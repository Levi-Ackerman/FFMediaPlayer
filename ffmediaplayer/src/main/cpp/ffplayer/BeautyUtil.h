//
// Created by Bytedance on 2021/2/18.
//

#ifndef FFMEDIAPLAYER_BEAUTYUTIL_H
#define FFMEDIAPLAYER_BEAUTYUTIL_H
#include <cstdint>
#include <WhiteTable.h>
#include <chrono>
#include <TimeClock.h>

extern "C" {
void skinWhite(uint8_t *input, uint8_t *output, int width, int height, int channels) {
  if (input == NULL || output == NULL) {
    return;
  }
  TimerClock clock;
  clock.update();
  const int size = width * height * channels;
  for (int i = 0; i < size; ++i) {
    output[i] = WhiteTable::getWhitePixelValue(input[i]);
  }
  auto t = clock.getTimerMilliSec();
  ALOGI("time cost :%lf", t);
}
#endif //FFMEDIAPLAYER_BEAUTYUTIL_H
}