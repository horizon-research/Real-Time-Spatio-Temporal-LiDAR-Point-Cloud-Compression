#ifndef UTILS_H
#define UTILS_H

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

#include "struct.h"
#include "config.h"

void load_pcloud(std::string& file_name,
                 std::vector<point_cloud>& pcloud_data);

void load_pcloud_xyz(std::string& file_name,
                     std::vector<point_cloud>& pcloud_data);

void pcloud2bin(std::string filename, std::vector<point_cloud>& pcloud_data);

void compute_cartesian_coord(const float radius, const float yaw,
    const float pitch, float& x, float& y, float& z,
    float pitch_precision, float yaw_precision);

std::string pcloud2string(std::vector<point_cloud>& pcloud_data);

void output_cloud(const std::vector<point_cloud>& pcloud, std::string filename);

std::vector<float>
output_normalize_cloud(const std::vector<point_cloud>& pcloud, std::string file_name);

void export_factor(cv::Mat& mat, const char* file_name, int precision);

void load_trace_data(const std::string& dir_path, std::vector<std::vector<point_cloud>>& frames,
    const std::string& input_format, int frame_cnt);

void load_trace_calib(const std::string& calib_path,
    std::vector<std::vector<float>>& moves, int frame_cnt);

#include "utils_impl.h"

#endif
