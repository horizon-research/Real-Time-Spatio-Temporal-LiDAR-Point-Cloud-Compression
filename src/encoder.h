#ifndef PCC_ENCODER_H
#define PCC_ENCODER_H

#include "utils.h"
#include "config.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <map>

namespace encoder
{

void single_channel_fit(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
     std::vector<cv::Vec4f>& coefficients, std::vector<int>& tile_fit_lengths,
     std::vector<float>& unfit_nums, std::vector<int>& unfit_code, 
     const float threshold, const int tile_size);

}
#endif
