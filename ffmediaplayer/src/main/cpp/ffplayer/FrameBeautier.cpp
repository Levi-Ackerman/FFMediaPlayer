//
// Created by Bytedance on 2021/2/19.
//

#include "FrameBeautier.h"
#include "WhiteTable.h"
#include <cmath>

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef max
#define max(a, b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef ClampToByte
#define  ClampToByte(v)  (((unsigned)(int)(v)) <(255) ? (v) : (v < 0) ? (0) : (255))
#endif
void skinSmoothing(unsigned char *input, unsigned char *output, int width, int height, int channels,
                   int smoothingLevel, int apply_skin_filter);

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
  uint8_t *data = (*outRGB)->data[0];
  for (int i = 0; i < width * height * 4; ++i) {
    //查表美白
    data[i] = WhiteTable::getWhitePixelValue(data[i]);
  }
//  磨皮
  skinSmoothing(data,data,width,height,4,10,0);
}


unsigned int skinDetection(unsigned char *rgb_src, int width, int height, int channels) {
  int stride = width * channels;
  int lastCol = width * channels - channels;
  int lastRow = height * stride - stride;
  unsigned int sum = 0;
  for (int y = 0; y < height; y++) {
    int cur_row = stride * y;
    int next_row = min(cur_row + stride, lastRow);
    unsigned char *next_scanLine = rgb_src + next_row;
    unsigned char *cur_scanLine = rgb_src + cur_row;
    for (int x = 0; x < width; x++) {
      int cur_col = x * channels;
      int next_col = min(cur_col + channels, lastCol);
      unsigned char *c00 = cur_scanLine + cur_col;
      unsigned char *c10 = cur_scanLine + next_col;
      unsigned char *c01 = next_scanLine + cur_col;
      unsigned char *c11 = next_scanLine + next_col;
      int r_avg = ((c00[0] + c10[0] + c01[0] + c11[0])) >> 2;
      int g_avg = ((c00[1] + c10[1] + c01[1] + c11[1])) >> 2;
      int b_avg = ((c00[2] + c10[2] + c01[2] + c11[2])) >> 2;
      if (r_avg >= 60 && g_avg >= 40 && b_avg >= 20 && r_avg >= b_avg && (r_avg - g_avg) >= 10 &&
          max(max(r_avg, g_avg), b_avg) - min(min(r_avg, g_avg), b_avg) >= 10) {
        sum++;
      }
    }
  }
  return sum;
}

void skinFilter(unsigned char *input, unsigned char *output, int width, int height, int channels) {
  int stride = width * channels;
  int lastCol = width * channels - channels;
  int lastRow = height * stride - stride;
  for (int y = 0; y < height; y++) {
    int cur_row = stride * y;
    int next_row = min(cur_row + stride, lastRow);
    unsigned char *next_scanOutLine = output + next_row;
    unsigned char *cur_scanOutLine = output + cur_row;
    unsigned char *scanOutLine = output + y * stride;
    unsigned char *scanInLine = input + y * stride;
    for (int x = 0; x < width; x++) {
      int cur_col = x * channels;
      int next_col = min(cur_col + channels, lastCol);
      unsigned char *c00 = cur_scanOutLine + cur_col;
      unsigned char *c10 = cur_scanOutLine + next_col;
      unsigned char *c01 = next_scanOutLine + cur_col;
      unsigned char *c11 = next_scanOutLine + next_col;
      int r_avg = ((c00[0] + c10[0] + c01[0] + c11[0])) >> 2;
      int g_avg = ((c00[1] + c10[1] + c01[1] + c11[1])) >> 2;
      int b_avg = ((c00[2] + c10[2] + c01[2] + c11[2])) >> 2;
      int is_skin = !(r_avg >= 60 && g_avg >= 40 && b_avg >= 20 && r_avg >= b_avg && (r_avg - g_avg) >= 10 &&
          max(max(r_avg, g_avg), b_avg) - min(min(r_avg, g_avg), b_avg) >= 10);
      if (is_skin)
        for (int c = 0; c < channels; ++c)
          scanOutLine[c] = scanInLine[c];
      scanOutLine += channels;
      scanInLine += channels;
    }
  }
}

void getOffsetPos(int *offsetPos, int length, int left, int right, int step) {
  if (offsetPos == NULL) return;
  if ((length < 0) || (left < 0) || (right < 0))
    return;
  for (int x = -left; x < length + right; x++) {
    int pos = x;
    int length2 = length + length;
    if (pos < 0) {
      do {
        pos += length2;
      } while (pos < 0);
    } else if (pos >= length2) {
      do {
        pos -= length2;
      } while (pos >= length2);
    }
    if (pos >= length)
      pos = length2 - 1 - pos;
    offsetPos[x + left] = pos * step;
  }
}


void skinDenoise(unsigned char *input, unsigned char *output, int width, int height, int channels, int radius,
                 int smoothingLevel) {
  if ((input == NULL) || (output == NULL)) return;
  if ((width <= 0) || (height <= 0)) return;
  if ((radius <= 0) || (smoothingLevel <= 0)) return;
  if ((channels != 1) && (channels != 3)) return;
  int windowSize = (2 * radius + 1) * (2 * radius + 1);
  int *colPower = (int *) malloc(width * channels * sizeof(int));
  int *colValue = (int *) malloc(width * channels * sizeof(int));
  int *rowPos = (int *) malloc((width + radius + radius) * channels * sizeof(int));
  int *colPos = (int *) malloc((height + radius + radius) * channels * sizeof(int));
  if ((colPower == NULL) || (colValue == NULL) || (rowPos == NULL) || (colPos == NULL)) {
    if (colPower) free(colPower);
    if (colValue) free(colValue);
    if (rowPos) free(rowPos);
    if (colPos) free(colPos);
    return;
  }
  int stride = width * channels;
  int smoothLut[256] = {0};
  float ii = 0.f;
  for (int i = 0; i <= 255; i++, ii -= 1.) {
    smoothLut[i] = (int) ((expf(ii * (1.0f / (smoothingLevel * 255.0f))) + (smoothingLevel * (i + 1)) + 1) * 0.5f);
    smoothLut[i] = max(smoothLut[i], 1);
  }
  getOffsetPos(rowPos, width, radius, radius, channels);
  getOffsetPos(colPos, height, radius, radius, stride);
  int *rowOffset = rowPos + radius;
  int *colOffSet = colPos + radius;
  for (int y = 0; y < height; y++) {
    unsigned char *scanInLine = input + y * stride;
    unsigned char *scanOutLine = output + y * stride;
    if (y == 0) {
      for (int x = 0; x < stride; x += channels) {
        int colSum[3] = {0};
        int colSumPow[3] = {0};
        for (int z = -radius; z <= radius; z++) {
          unsigned char *sample = input + colOffSet[z] + x;
          for (int c = 0; c < channels; ++c) {
            colSum[c] += sample[c];
            colSumPow[c] += sample[c] * sample[c];
          }
        }
        for (int c = 0; c < channels; ++c) {
          colValue[x + c] = colSum[c];
          colPower[x + c] = colSumPow[c];
        }
      }
    } else {
      unsigned char *lastCol = input + colOffSet[y - radius - 1];
      unsigned char *nextCol = input + colOffSet[y + radius];
      for (int x = 0; x < stride; x += channels) {
        for (int c = 0; c < channels; ++c) {
          colValue[x + c] -= lastCol[x + c] - nextCol[x + c];
          colPower[x + c] -= lastCol[x + c] * lastCol[x + c] - nextCol[x + c] * nextCol[x + c];
        }
      }
    }
    int prevSum[3] = {0};
    int prevPowerSum[3] = {0};
    for (int z = -radius; z <= radius; z++) {
      int index = rowOffset[z];
      for (int c = 0; c < channels; ++c) {
        prevSum[c] += colValue[index + c];
        prevPowerSum[c] += colPower[index + c];
      }
    }
    for (int c = 0; c < channels; ++c) {
      const int mean = prevSum[c] / windowSize;
      const int diff = mean - scanInLine[c];
      const int edge = ClampToByte(diff);
      const int masked_edge = (edge * scanInLine[c] + (256 - edge) * mean) >> 8;
      const int var = (prevPowerSum[c] - mean * prevSum[c]) / windowSize;
      const int out = masked_edge - diff * var / (var + smoothLut[scanInLine[c]]);
      scanOutLine[c] = ClampToByte(out);
    }
    scanInLine += channels;
    scanOutLine += channels;
    for (int x = 1; x < width; x++) {
      int lastRow = rowOffset[x - radius - 1];
      int nextRow = rowOffset[x + radius];
      for (int c = 0; c < channels; ++c) {
        prevSum[c] = prevSum[c] - colValue[lastRow + c] + colValue[nextRow + c];
        prevPowerSum[c] = prevPowerSum[c] - colPower[lastRow + c] + colPower[nextRow + c];
        const int mean = prevSum[c] / windowSize;
        const int diff = mean - scanInLine[c];
        const int edge = ClampToByte(diff);
        const int masked_edge = (edge * scanInLine[c] + (256 - edge) * mean) >> 8;
        const int var = (prevPowerSum[c] - mean * prevSum[c]) / windowSize;
        const int out = masked_edge - diff * var / (var + smoothLut[scanInLine[c]]);
        scanOutLine[c] = ClampToByte(out);
      }
      scanInLine += channels;
      scanOutLine += channels;
    }
  }
  if (colPower) free(colPower);
  if (colValue) free(colValue);
  if (rowPos) free(rowPos);
  if (colPos) free(colPos);
}


void skinSmoothing(unsigned char *input, unsigned char *output, int width, int height, int channels,
                   int smoothingLevel, int apply_skin_filter) {
  if (input == NULL || output == NULL || width == 0 || height == 0 || channels == 1)
    return;
  //1.detect skin color, adapt radius according to skin color ratio
  unsigned int skinSum = skinDetection(input, width, height, channels);
  float skin_rate = skinSum / (float) (width * height) * 100;
  int radius = min(width, height) / skin_rate + 1;
  //2.perform edge detection to obtain a edge map && smoothing level for apply skin denoise
  skinDenoise(input, output, width, height, channels, radius, smoothingLevel);
  //3.re-detect skin color based on the denoise results, filtered non-skin areas
  if (apply_skin_filter)
    skinFilter(input, output, width, height, channels);
}
