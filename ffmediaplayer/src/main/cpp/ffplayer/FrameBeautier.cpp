//
// Created by Bytedance on 2021/2/19.
//

#include "FrameBeautier.h"

FrameBeautier::FrameBeautier(AVCodecContext *videoCodecContext) {

  sws_ctx = sws_getContext(videoCodecContext->width, videoCodecContext->height,
                           videoCodecContext->pix_fmt,
                           videoCodecContext->width, videoCodecContext->height, AV_PIX_FMT_RGBA,
                           SWS_BILINEAR, NULL, NULL, NULL);

  width = videoCodecContext->width;
  height = videoCodecContext->height;
}

void FrameBeautier::beauty(AVFrame *inYUV, AVFrame **outRGB) {
  *outRGB = av_frame_alloc();
  av_frame_copy_props(*outRGB, inYUV);
  (*outRGB)->height = height;
  (*outRGB)->width = width;
  (*outRGB)->format = AV_PIX_FMT_RGBA;
  av_image_alloc((*outRGB)->data, (*outRGB)->linesize, width, height, AV_PIX_FMT_RGBA, 1);
  sws_scale(sws_ctx, inYUV->data, inYUV->linesize, 0, height, (*outRGB)->data, (*outRGB)->linesize);

  skinWhite((*outRGB)->data[0], (*outRGB)->data[0], width, height, 4);
}
