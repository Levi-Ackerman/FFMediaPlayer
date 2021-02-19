//
// Created by llm on 20-7-17.
//

#ifndef FFMEDIAPLAYER_SINKFRAMEPAD_H
#define FFMEDIAPLAYER_SINKFRAMEPAD_H

#include "FFPad.h"
#include "safe_queue.h"
#include "FrameBeautier.h"

class SinkFramePad: public FFPad {
 public:
  SinkFramePad(pad_type padType, pad_media_type mediaType, FrameBeautier *beautier = nullptr);
  virtual ~SinkFramePad();
  virtual void addData(void *frame);
  virtual void *getData();
  virtual bool isDataBufferEmpty();

private:
    SafeQueue<AVFrame *> frames;
    FrameBeautier *m_beautier;
};

#endif //FFMEDIAPLAYER_SINKFRAMEPAD_H
