//
// Created by Bytedance on 2021/2/19.
//

#ifndef FFMEDIAPLAYER_WHITETABLE_H
#define FFMEDIAPLAYER_WHITETABLE_H

class WhiteTable{
 private:
  static int* init_white_list();
  static int* white_list ;
 public:
  inline static int getWhitePixelValue(int pixel) {
    return white_list[pixel];
  }
};


#endif //FFMEDIAPLAYER_WHITETABLE_H
