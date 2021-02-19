//
// Created by Bytedance on 2021/2/18.
//

#ifndef FFMEDIAPLAYER_BEAUTYUTIL_H
#define FFMEDIAPLAYER_BEAUTYUTIL_H
#include <cstdint>
#include <WhiteTable.h>
#include <chrono>
#include <TimeClock.h>
#include <thread>
#include <future>

static const int THREAD_NUM = 4;
static std::future<void> futs[THREAD_NUM];

extern "C" {
void skinWhite(uint8_t *input, uint8_t *output, int width, int height, int channels) {
  if (input == NULL || output == NULL) {
    return;
  }
//  TimerClock clock;
  const int size = width * height * channels;
  const int section_length = size / THREAD_NUM;
  for (int i = 0; i < THREAD_NUM; ++i) {
    futs[i] =
        std::move(std::async(std::launch::async, [](int i, int size, uint8_t *input, uint8_t *output,int section_length) {
          int start = i * section_length;
          int end = min(i * section_length + section_length, size);
          for (int j = start; j < end; ++j) {
            output[j] = WhiteTable::getWhitePixelValue(input[j]);
          }
        }, i, size, input, output,section_length));
  }
  for (int i = 0; i < THREAD_NUM; ++i) {
    futs[i].get();
  }

//  auto t = clock.getTimerMilliSec();
}
#endif //FFMEDIAPLAYER_BEAUTYUTIL_H
}