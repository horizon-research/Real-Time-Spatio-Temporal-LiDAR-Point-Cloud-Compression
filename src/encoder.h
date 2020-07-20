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

void encode_occupation_mat(cv::Mat& img, cv::Mat& occ_mat, int tile_size,
                           const int* idx_sizes);

double single_channel_encode(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
                             std::vector<cv::Vec4f>& coefficients,
                             std::vector<float>& unfit_nums,
                             std::vector<int>& tile_fit_lengths, 
                             const float threshold, const int tile_size);


double multi_channel_encode(std:: vector<cv::Mat*>& imgs, cv::Mat& b_mat, 
                            const int* idx_sizes,
                            std::vector<cv::Vec4f>& coefficients,
                            std::vector<std::vector<float>>& plane_offsets,
                            std::vector<int>& tile_fit_lengths, 
                            const float threshold, const int tile_size);

}
#endif
