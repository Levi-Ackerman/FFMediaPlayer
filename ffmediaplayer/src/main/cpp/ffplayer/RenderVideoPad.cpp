//
// Created by llm on 20-6-11.
//

#include "RenderVideoPad.h"

RenderVideoPad::RenderVideoPad() {
    padType = PAD_SINK;
    mediaType = PAD_VIDEO;

    frames.setReleaseCallback(releaseAVFrame);
    frames.setWork(1);
}

void RenderVideoPad::addData(void *frame) {
 //   ALOGE("RenderVideoPad::addData ");
    while (frames.size() > 100) {
        av_usleep(10 * 1000);
        continue;
    }

 //   ALOGE("RenderVideoPad::addData 2");
    frames.push((AVFrame *) frame);
}

RenderVideoPad::~RenderVideoPad() {
    ALOGE("RenderVideoPad::~RenderVideoPad()");
    frames.clear();
}

void *RenderVideoPad::getData() {
    AVFrame *frame = 0;
  //  ALOGE("RenderVideoPad::getData() frames.size:%d",frames.size()  );
    if (frames.size() > 0) {
        frames.pop(frame);
    }
    return frame;
}

bool RenderVideoPad::isDataBufferEmpty() {
    if (frames.empty()) {
        return true;
    } else {
        return false;
    }
}
