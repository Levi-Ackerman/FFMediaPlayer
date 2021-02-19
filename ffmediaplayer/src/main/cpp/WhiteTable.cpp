//
// Created by Bytedance on 2021/2/19.
//
#include "WhiteTable.h"
#include <cmath>
#include <FFLog.h>

const int beta = 5;
const int beta_1 = beta -1;
const float log_beta = log(beta);

int *WhiteTable::white_list = WhiteTable::init_white_list();

int *WhiteTable::init_white_list() {
  auto pixels = new int[256];
  for (int i = 0; i < 256; ++i) {
    pixels[i] = log(1.0 * i /255 * beta_1 + 1) /log_beta * 255;
  }
  return pixels;
}

