#ifndef PCC_ENCODER_H
#define PCC_ENCODER_H

#include "utils.h"
#include "config.h"
#include "interpolation.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <map>

namespace encoder
{

float characterize_occupation(cv::Mat& occ_m, int tile_size, const int* mat_sizes, float pcloud_size);

std::pair<float, cv::Vec4f> multi_channel_compression(std::vector<cv::Mat*>& img, 
          std::vector<cv::Mat*>& r_img, const int * mat_sizes, float threshold,
          int tile_size, int channel, cv::Mat& factor_m);

}
#endif
