#include "utils.h"
#include "analysis.h"

bool verbose = 1;

std::vector<point_cloud> cluster_pcloud(std::vector<point_cloud>& pcloud_data,
                                        int num_of_center,
                                        std::vector<std::vector<point_cloud>>& clusters) {
  alglib::clusterizerstate state;
  alglib::kmeansreport report;
  printf("[Cluster]: start to cluster the points.\n");
  alglib::real_2d_array xyz = pcloud2string(pcloud_data).c_str(); 

  // create cluster and run k-means
  alglib::clusterizercreate(state);
  alglib::clusterizersetpoints(state, xyz, 2);
  alglib::clusterizersetkmeanslimits(state, 2, 1000);
  printf("[Cluster]: start calculation.\n");
  alglib::clusterizerrunkmeans(state, num_of_center, report);
  printf("[Cluster]: termination ends wih %d\n", int(report.terminationtype));
  auto center_points = report.c;
  std::vector<point_cloud> centers = {};
  for (int i = 0; i < num_of_center; i++) {
    printf("%f %f %f\n", center_points[i][0],
           center_points[i][1], center_points[i][2]);

    centers.push_back(point_cloud(center_points[i][0],
                                  center_points[i][1],
                                  center_points[i][2], 0));
  }

  for (int i = 0; i < report.cidx.length(); i++) {
    auto idx = report.cidx[i];
    auto x = pcloud_data[i].x - center_points[idx][0];
    auto y = pcloud_data[i].y - center_points[idx][1];
    auto z = pcloud_data[i].z - center_points[idx][2];
    clusters[idx].push_back(point_cloud(x, y, z, 0));
  }
  return centers;
}


// convert diff mat (in float) to image mat
// basically, we need to convert scale up (down)
// diff mat to 0-255 image mat and feed in video compression
cv::Mat convert_diff_2img(cv::Mat& diff_mat, float& min_val, float& max_val) {
  min_val = 0; max_val = 0;
  for (int i = 0; i < diff_mat.rows; i++) {
    for (int j = 0; j < diff_mat.cols; j++) {
      min_val = std::min(min_val, diff_mat.at<float>(i,j));
      max_val = std::max(max_val, diff_mat.at<float>(i,j));
    }
  }
  auto base = max_val - min_val;
  cv::Mat ret = cv::Mat(diff_mat.rows, diff_mat.cols, CV_8UC1, cv::Scalar(0));
  for (int i = 0; i < diff_mat.rows; i++)
    for (int j = 0; j < diff_mat.cols; j++)
      ret.at<uint8_t>(i, j) = (uint8_t)(255*(diff_mat.at<float>(i, j)-min_val)/(base));

  return ret;
}


// the reverse function vs. convert_diff_2img
cv::Mat convert_img_2diff(cv::Mat& img, float& min_val, float& max_val) {
  
  auto base = max_val - min_val;
  cv::Mat ret = cv::Mat(img.rows, img.cols, CV_32FC1, cv::Scalar(0));
  for (int i = 0; i < img.rows; i++)
    for (int j = 0; j < img.cols; j++)
      ret.at<float>(i, j) = (float)img.at<uint8_t>(i, j)/255.0f*base+min_val;

  return ret;
}
