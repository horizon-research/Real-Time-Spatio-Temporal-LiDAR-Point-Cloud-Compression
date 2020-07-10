#ifndef ANALYSIS_H
#define ANALYSIS_H

/* std library */
#include <algorithm>
#include <cmath>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include <map>
#include <string>

/* openCV library */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>

#include "utils.h"
#include "struct.h"
#include "config.h"

template <class T>
float characterize_neighbors(cv::Mat& img);

void compare_two_mats(std::vector<cv::Mat>& arr1, std::vector<cv::Mat>& arr2);

std::vector<point_cloud> cluster_pcloud(std::vector<point_cloud>& pcloud_data,
                                        int num_of_center,
                                        std::vector<std::vector<point_cloud>>& clusters);

cv::Mat convert_diff_2img(cv::Mat& diff_mat, float& min_val, float& max_val);

cv::Mat convert_img_2diff(cv::Mat& img, float& min_val, float& max_val);

#include "analysis_impl.h"

#endif
