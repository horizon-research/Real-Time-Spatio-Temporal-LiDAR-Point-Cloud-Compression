#ifndef PCC_DECODER_H
#define PCC_DECODER_H

#include "utils.h"
#include "config.h"
#include <cmath>
#include <string>
#include <algorithm>
#include <map>

namespace decoder
{

double single_channel_decode(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
                             std::vector<cv::Vec4f>& coefficients, cv::Mat& occ_mat,
                             std::vector<int>& tile_fit_lengths,
                             std::vector<float>& unfit_nums, int tile_size, cv::Mat& org);

}
#endif
