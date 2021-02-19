//
// Created by Bytedance on 2021/2/19.
//

#ifndef FFMEDIAPLAYER_RGBAFRAME_H
#define FFMEDIAPLAYER_RGBAFRAME_H
#include <cstdint>
typedef struct RgbaFrame {
  uint8_t *dst_data[4];
  int dst_linesize[4];
  int repeat_pict;
  double best_effort_timestamp;
} RgbaFrame;
#endif //FFMEDIAPLAYER_RGBAFRAME_H
