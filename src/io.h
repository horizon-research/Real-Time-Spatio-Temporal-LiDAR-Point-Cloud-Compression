#ifndef PCC_IO_H
#define PCC_IO_H

/* std library */
#include <cmath>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>

/* openCV library */
#include <opencv2/highgui/highgui.hpp>

#include "struct.h"
#include "config.h"

void export_b_mat(cv::Mat& b_mat, std::string filename);

void import_b_mat(cv::Mat& b_mat, std::string filename);

void export_coefficients(std::vector<cv::Vec4f>& coefficients,
                         std::string filename);

void import_coefficients(std::vector<cv::Vec4f>& coefficients,
                         std::string filename);

void export_occ_mat(cv::Mat& occ_mat, std::string filename);

void import_occ_mat(cv::Mat& occ_mat, std::string filename);

void export_unfit_nums(std::vector<float>& data, std::string filename);

void import_unfit_nums(std::vector<float>& data, std::string filename);

void export_tile_fit_lengths(std::vector<int>& data, std::string filename);

void import_tile_fit_lengths(std::vector<int>& data, std::string filename);

void export_plane_offsets(std::vector<std::vector<float>>& data,
                          std::string filename);

void import_plane_offsets(std::vector<std::vector<float>>& data,
                          std::string filename, int size);

void export_filenames(const std::vector<std::string>& data, std::string filename);

void import_filenames(std::vector<std::string>& data, std::string filename);


#include "io_impl.h"

#endif
