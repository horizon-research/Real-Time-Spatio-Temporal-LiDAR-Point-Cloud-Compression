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

#endif
