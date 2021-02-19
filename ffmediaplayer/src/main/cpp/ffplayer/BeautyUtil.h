//
// Created by Bytedance on 2021/2/18.
//

#ifndef FFMEDIAPLAYER_BEAUTYUTIL_H
#define FFMEDIAPLAYER_BEAUTYUTIL_H
#include <cstdint>
#include "sys/time.h"
#include <cmath>
#include <thread>
#include <chrono>
#include <future>

static const int beta = 5;
static const int beta_1 = beta - 1;
static const float logBeta = log(beta);
static const float fmap[] =
    {0.0000, 0.0039, 0.0078, 0.0117, 0.0156, 0.0195, 0.0234, 0.0273, 0.0312, 0.0352, 0.0391, 0.0430, 0.0469, 0.0508,
     0.0547, 0.0586, 0.0625, 0.0664, 0.0703, 0.0742, 0.0781, 0.0820, 0.0859, 0.0898, 0.0938, 0.0977, 0.1016, 0.1055,
     0.1094, 0.1133, 0.1172, 0.1211, 0.1250, 0.1289, 0.1328, 0.1367, 0.1406, 0.1445, 0.1484, 0.1523, 0.1562, 0.1602,
     0.1641, 0.1680, 0.1719, 0.1758, 0.1797, 0.1836, 0.1875, 0.1914, 0.1953, 0.1992, 0.2031, 0.2070, 0.2109, 0.2148,
     0.2188, 0.2227, 0.2266, 0.2305, 0.2344, 0.2383, 0.2422, 0.2461, 0.2500, 0.2539, 0.2578, 0.2617, 0.2656, 0.2695,
     0.2734, 0.2773, 0.2812, 0.2852, 0.2891, 0.2930, 0.2969, 0.3008, 0.3047, 0.3086, 0.3125, 0.3164, 0.3203, 0.3242,
     0.3281, 0.3320, 0.3359, 0.3398, 0.3438, 0.3477, 0.3516, 0.3555, 0.3594, 0.3633, 0.3672, 0.3711, 0.3750, 0.3789,
     0.3828, 0.3867, 0.3906, 0.3945, 0.3984, 0.4023, 0.4062, 0.4102, 0.4141, 0.4180, 0.4219, 0.4258, 0.4297, 0.4336,
     0.4375, 0.4414, 0.4453, 0.4492, 0.4531, 0.4570, 0.4609, 0.4648, 0.4688, 0.4727, 0.4766, 0.4805, 0.4844, 0.4883,
     0.4922, 0.4961, 0.5000, 0.5039, 0.5078, 0.5117, 0.5156, 0.5195, 0.5234, 0.5273, 0.5312, 0.5352, 0.5391, 0.5430,
     0.5469, 0.5508, 0.5547, 0.5586, 0.5625, 0.5664, 0.5703, 0.5742, 0.5781, 0.5820, 0.5859, 0.5898, 0.5938, 0.5977,
     0.6016, 0.6055, 0.6094, 0.6133, 0.6172, 0.6211, 0.6250, 0.6289, 0.6328, 0.6367, 0.6406, 0.6445, 0.6484, 0.6523,
     0.6562, 0.6602, 0.6641, 0.6680, 0.6719, 0.6758, 0.6797, 0.6836, 0.6875, 0.6914, 0.6953, 0.6992, 0.7031, 0.7070,
     0.7109, 0.7148, 0.7188, 0.7227, 0.7266, 0.7305, 0.7344, 0.7383, 0.7422, 0.7461, 0.7500, 0.7539, 0.7578, 0.7617,
     0.7656, 0.7695, 0.7734, 0.7773, 0.7812, 0.7852, 0.7891, 0.7930, 0.7969, 0.8008, 0.8047, 0.8086, 0.8125, 0.8164,
     0.8203, 0.8242, 0.8281, 0.8320, 0.8359, 0.8398, 0.8438, 0.8477, 0.8516, 0.8555, 0.8594, 0.8633, 0.8672, 0.8711,
     0.8750, 0.8789, 0.8828, 0.8867, 0.8906, 0.8945, 0.8984, 0.9023, 0.9062, 0.9102, 0.9141, 0.9180, 0.9219, 0.9258,
     0.9297, 0.9336, 0.9375, 0.9414, 0.9453, 0.9492, 0.9531, 0.9570, 0.9609, 0.9648, 0.9688, 0.9727, 0.9766, 0.9805,
     0.9844, 0.9883, 0.9922, 0.9961};

extern "C" {
void skinWhite(uint8_t *input, uint8_t *output, int width, int height, int channels) {
  if (input == NULL || output == NULL) {
    return;
  }
  const int size = width * height * channels;

  const int THREAD_NUM = 9;
  std::future<void > futs[THREAD_NUM];
  for (int i = 0; i < THREAD_NUM; ++i) {
    futs[i] = std::move(std::async(std::launch::async, [](int mod, int size, uint8_t *input, uint8_t *output) {
      for (int j = mod; j < size; j += THREAD_NUM) {
        output[j] = log(beta_1 * fmap[input[j]] + 1) / logBeta * 255;
      }
    }, i,size, input, output));
  }
  for (int i = 0; i < THREAD_NUM; ++i) {
    futs[i].get();
  }
}
#endif //FFMEDIAPLAYER_BEAUTYUTIL_H
}