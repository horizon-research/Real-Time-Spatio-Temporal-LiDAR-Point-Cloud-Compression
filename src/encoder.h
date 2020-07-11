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

double single_channel_encode(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
                             std::vector<cv::Vec4f>& coefficients, cv::Mat& occ_mat,
                             std::vector<float>& unfit_nums,
                             std::vector<int>& tile_fit_lengths, 
                             const float threshold, const int tile_size);

}
#endif
