#ifndef PCC_MODULE
#define PCC_MODULE

/* std library */
#include <algorithm>
#include <bits/stdc++.h>
#include <chrono>
#include <cmath>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <map>
#include <string>

/* openCV library */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// #include <opencv/highgui.h>

/* boost library */
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

/* my own library */
#include "struct.h"
#include "config.h"
#include "utils.h"

double map_projection(cv::Mat& f_mat, std::vector<point_cloud>& pcloud_data,
                      float pitch_precision, float yaw_precision, char option);

template <class T>
T average(std::vector<T>* vec) {
  return std::accumulate(vec->begin(), vec->end(), 0.0)/vec->size();
}

void print_pcc_res(PccResult& pcc_res);

#endif
