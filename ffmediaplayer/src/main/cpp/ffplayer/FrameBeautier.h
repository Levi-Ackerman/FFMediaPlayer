//
// Created by Bytedance on 2021/2/19.
//

#ifndef FFMEDIAPLAYER_FRAMEBEAUTIER_H
#define FFMEDIAPLAYER_FRAMEBEAUTIER_H

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"

};

class FrameBeautier {
 private:
  SwsContext *sws_ctx = 0;
  int width;
  int height;

 public:
  FrameBeautier(AVCodecContext *pContext);

  void beauty(AVFrame* inYUV, AVFrame** outRGB);
};


#endif //FFMEDIAPLAYER_FRAMEBEAUTIER_H
