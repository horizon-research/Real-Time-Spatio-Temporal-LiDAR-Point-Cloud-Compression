#include "utils.h"
#include <algorithm>

namespace encoder
{
/*
 * filter out points are essentially zero.
 * */
void filter_vals(cv::Mat& img, std::vector<std::pair<float, float>>& idx_vals,
    std::vector<float>& range_vals, int c_idx, int r_idx, int len, int height) {
  
  for (int r = r_idx; r < std::min(height+r_idx, img.rows); r++) {
    for (int c = c_idx; c < std::min(len+c_idx, img.cols); c++) {
      auto vec = img.at<cv::Vec4f>(r, c);
      if (vec[0] > 0.0){
        idx_vals.push_back(std::make_pair(float(r-r_idx), float(c-c_idx)));
        range_vals.push_back(vec[0]);
      }
    }
  }
  return;
}

/*
 * fit 3d points in a plane.
 * */
cv::Vec4f plane_fitting(std::vector<std::pair<float, float>>& idx_vals,
                                    std::vector<float>& range_vals) {
  if (idx_vals.size() < 3) throw -1;

  cv::Vec3f sum(0, 0, 0);
  for (int i = 0; i < idx_vals.size(); i++) {
    float x = range_vals[i];
    float y = idx_vals[i].first;
    float z = idx_vals[i].second;
    sum[0] += x;
    sum[1] += y;
    sum[2] += z;
  }
  cv::Vec3f centroid;
  centroid[0] = sum[0] / idx_vals.size();
  centroid[1] = sum[1] / idx_vals.size();
  centroid[2] = sum[2] / idx_vals.size();

  float xx = 0, xy = 0, xz = 0;
  float yy = 0, yz = 0, zz = 0;

  for (int i = 0; i < idx_vals.size(); i++) {
    float r_x = range_vals[i] - centroid[0];
    float r_y = idx_vals[i].first - centroid[1];
    float r_z = idx_vals[i].second - centroid[2];

    xx += r_x * r_x;
    xy += r_x * r_y;
    xz += r_x * r_z;
    yy += r_y * r_y;
    yz += r_y * r_z;
    zz += r_z * r_z;
  }

  xx /= idx_vals.size();
  xy /= idx_vals.size();
  xz /= idx_vals.size();
  yy /= idx_vals.size();
  yz /= idx_vals.size();
  zz /= idx_vals.size();

  cv::Vec3f dir(0, 0, 0);

  float det_x = yy*zz - yz*yz;
  cv::Vec3f x_axis_dir(det_x, (xz*yz - xy*zz), (xy*yz - xz*yy));
  float x_weight = det_x * det_x;

  if ((dir[0] * x_axis_dir[0] +
       dir[1] * x_axis_dir[1] + 
       dir[2] * x_axis_dir[2]) < 0) {
    x_weight = - x_weight;
  }
  dir[0] += x_axis_dir[0]*x_weight;
  dir[1] += x_axis_dir[1]*x_weight;
  dir[2] += x_axis_dir[2]*x_weight;

  float det_y = xx*zz - xz*xz;
  cv::Vec3f y_axis_dir(xz*yz - xy*zz, det_y, xy*xz - yz*xx);
  float y_weight = det_y * det_y;

  if ((dir[0] * y_axis_dir[0] +
       dir[1] * y_axis_dir[1] + 
       dir[2] * y_axis_dir[2]) < 0) {
    y_weight = -y_weight;
  }
  dir[0] += y_axis_dir[0]*y_weight;
  dir[1] += y_axis_dir[1]*y_weight;
  dir[2] += y_axis_dir[2]*y_weight;

  float det_z = xx*yy - xy*xy;
  cv::Vec3f z_axis_dir(xy*yz - xz*yy, xy*xz - yz*xx, det_z);
  float z_weight = det_z * det_z;


  if ((dir[0] * z_axis_dir[0] +
       dir[1] * z_axis_dir[1] + 
       dir[2] * z_axis_dir[2]) < 0) {
    z_weight = - z_weight;
  }

  dir[0] += z_axis_dir[0]*z_weight;
  dir[1] += z_axis_dir[1]*z_weight;
  dir[2] += z_axis_dir[2]*z_weight;

  cv::Vec4f factors(0.f, 0.f, 0.f, 0.f);
  float norm = sqrt(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);

  if (norm == 0.f) throw -1;

  dir[0] /=norm;
  dir[1] /=norm;
  dir[2] /=norm;


  factors[0] = dir[0];
  factors[1] = dir[1];
  factors[2] = dir[2];
  factors[3] = -(dir[0]*centroid[0] + dir[1]*centroid[1] + dir[2]*centroid[2]);

  if (factors[2] == 0.f || factors[3] == 0.f) throw -1;
  
  return cv::Vec4f(factors);
}

bool test_tile(cv::Mat& img, const cv::Vec4f& c, float threshold,
               int c_idx, int r_idx, int len, int height, std::vector<float>& avgs) {
  int cnt = 0;
  float sum = 0;
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < len; i++) {
      auto vec = img.at<cv::Vec4f>(r_idx+j, c_idx+i);
      float actual_value = vec[0];
      if (actual_value > 0) {
        cnt++;
        sum += c[0]*vec[0] + c[1]*j + c[2]*i;
      }
    }
  }
  float avg = (cnt == 0) ? 0 : sum/cnt;
  avgs.push_back(avg);

  for (int i = 0; i < len; i++) {
    for (int j = 0; j < height; j++) {
      auto vec = img.at<cv::Vec4f>(r_idx+j, c_idx+i);
      float actual_value = vec[0];
      if (actual_value <= 0) continue;

      float val = fabs(-avg/(c[0]*vec[0] + c[1]*j + c[2]*i))*vec[0];
      float diff = fabs(val - actual_value);

      if (diff > threshold) return false;
    }
  }
  return true;
}

bool comp_result(cv::Mat& img, cv::Mat& comp_mat, const cv::Vec4f& c, float threshold,
                 int c_idx, int r_idx, int len, int height, int channel) {
  int cnt = 0;
  float sum = 0;
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < len; i++) {
      auto vec = img.at<cv::Vec4f>(r_idx+j, c_idx+i);
      float actual_value = vec[0];
      if (actual_value > 0) {
        cnt++;
        sum += c[0]*vec[0] + c[1]*j + c[2]*i;
      }
    }
  }

  float avg = sum/cnt;
  int unfit_cnt = 0;

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < len; i++) {
      auto vec = img.at<cv::Vec4f>(r_idx+j, c_idx+i);
      float actual_value = vec[0];
      if (actual_value == 0) {
        comp_mat.at<cv::Vec2f>(channel, r_idx+j, c_idx+i) = cv::Vec2f(0, 0);
        continue;
      }
      float val = fabs(avg/(c[0]*vec[0] + c[1]*j + c[2]*i))*vec[0];
  
      float diff = fabs(val - actual_value);
      if (diff > threshold) {
        // std::cout << val << " " << vec[0] << std::endl;
        comp_mat.at<cv::Vec2f>(channel, r_idx+j, c_idx+i) = cv::Vec2f(val, vec[0]);
      }
    }
  }
  return true;
}

bool check_mat(cv::Mat& img, const cv::Vec4f& c, float threshold,
               int c_idx, int r_idx, int len, int height) {
  if (c[0] == 0.f && c[1] == 0.f && c[2] == 0.f)
    return false;

  for (int j = 0; j < height; j++) {
    for (int i = 0; i < len; i++) {
      auto vec = img.at<cv::Vec4f>(r_idx+j, c_idx+i);
      float actual_value = vec[0];
      if (actual_value > 0) {
        float val = fabs(-c[3]/(c[0]*vec[0] + c[1]*j + c[2]*i))*vec[0];
        float diff = fabs(val - actual_value);
        if (diff > threshold) return false;
      }
    }
  }
  return true;
}

int delta_coding(cv::Mat& mat, const int* idx_sizes, int tile_size) {
  int cnt = 0;
  int total_cnt = 0;
  for (int i = 0; i < idx_sizes[2]; i += tile_size) {
    for (int j = 0; j < idx_sizes[1]; j += tile_size) {
      for (int k = 0; k < idx_sizes[0]; k++) {
        bool check = false;
        for (int ii = 0; ii < tile_size; ii++) {
          for (int jj = 0; jj < tile_size; jj++) {
            auto val = mat.at<cv::Vec2f>(k, j+jj, i+ii);
            if (val[0] > 0.f) {
              check = true;
              total_cnt++;
            }
          }
        }
        if (check) cnt++;
      }
    }
  }
  std::cout << "[DELTA] The unfitted tiles: " << cnt << std::endl;
  std::cout << "Delta coding: : " << total_cnt/(tile_size*tile_size) << std::endl;
  return total_cnt;
}

int copy_unfit_points(std::vector<cv::Mat*>& img, std::vector<cv::Mat*>& r_img,
                      const int *mat_sizes, int tile_size) {
  int unfitted_tile = 0;
  for (int k = 0; k < mat_sizes[0]; k++) {
    for (int r_idx = 0; r_idx < mat_sizes[1]; r_idx += tile_size) {
      for (int c_idx = 0; c_idx < mat_sizes[2]; c_idx += tile_size) {
        bool check = false;
        for (int ii = 0; ii < tile_size; ii++) {
          for (int jj = 0; jj < tile_size; jj++) {
            auto vec = img[k]->at<cv::Vec4f>(r_idx+ii, c_idx+jj);
            if (vec[0] > 0.01f) {
              check = true;
              r_img[k]->at<cv::Vec4f>(r_idx+ii, c_idx+jj) = cv::Vec4f(vec);
            }
          }
        }
        if (check) unfitted_tile++;
      }
    }
  }
  std::cout << "[COPY] The unfitted tiles: " << unfitted_tile << std::endl;
  return unfitted_tile;
}

float characterize_occupation(cv::Mat& occ_m, int tile_size, const int* mat_sizes, float pcloud_size) {
  std::map<int, int> m;
  for (int i = 0; i < occ_m.rows/tile_size; i++) {
    for (int j = 0; j < occ_m.cols/tile_size; j++) {
      int val = 0;
      for (int ii = 0; ii < tile_size; ii++) {
        for (int jj = 0; jj < tile_size; jj++) {
          val = val * 2;
          if (occ_m.at<cv::Vec4f>(i*tile_size+ii, j*tile_size+jj)[0] > 0.f)  {
            val += 1;
          };
        }
      }
      if (m.find(val) == m.end()) {
        m[val] = 1;
      } else {
        m[val]++;
      }
    }
  }

  std::vector<int> vec = {};
  for (auto itr =  m.begin(); itr != m.end(); itr++) {
    vec.push_back(itr->second);
  }

  int bits = 0;
  std::sort(vec.begin(), vec.end(), std::greater<int>());
  for (int i = 0; i < vec.size(); i++) {
    if (i < 8)
      bits += (2+3)*vec[i];
    else if (i < 40 && i >=8)
      bits += (2+5)*vec[i];
    else if (i >= 40 && i < 168)
      bits += (2+7)*vec[i];
    else
      bits += vec[i]*18;
  }

  std::cout << "additional bits for occupation map("<< vec.size() << "): " << bits
            << " : " << pcloud_size  << " = " << bits/pcloud_size << std::endl;
 
  return bits/pcloud_size; 
}

cv::Mat normalized_img(cv::Mat& img, const int tile_size) {
  float max_r = 0;
  for (int r = 0; r < img.rows; r++) {
    for (int c = 0; c < img.cols; c++) {
      max_r = (max_r > img.at<cv::Vec4f>(r, c)[0]) ? max_r :  img.at<cv::Vec4f>(r, c)[0];
    }
  }

  cv::Mat norm_img(img.rows, img.cols, CV_8UC1, cv::Scalar(0));

  for (int r = 0; r < img.rows; r += tile_size) {
    for (int c = 0; c < img.cols; c += tile_size) {
      float sum = 0;
      for (int rr = 0; rr < tile_size; rr++) {
        for (int cc = 0; cc < tile_size; cc++) {
          int val = (int)(img.at<cv::Vec4f>(r, c)[0]/max_r*255);
          norm_img.at<uint8_t>(r+rr, c+cc) = val%256;
          sum += img.at<cv::Vec4f>(r+rr, c+cc)[0];
        }
      }
      if (sum < 1.0f) {
        for (int rr = 0; rr < tile_size; rr++) {
          for (int cc = 0; cc < tile_size; cc++) {
            norm_img.at<uint8_t>(r+rr, c+cc) = 255;
          }
        }
      }
    }
  }
  return norm_img;
}

void export_mats(std::vector<cv::Mat*>& img, int tile_size) {
  for (int k = 0; k < img.size(); k++) {
    char img_file[64];
    sprintf(img_file, "indi_%06d.jpg", k);
    auto norm_img = normalized_img(*(img[k]), tile_size);
    std::vector<int> params = {CV_IMWRITE_JPEG_QUALITY, 100};
    cv::imwrite(img_file, norm_img, params);
  }
}

/*
 * test and merge the next tiles with previous fitted coefficients 
 * */
bool merge(cv::Mat& img, int c_idx, int r_idx, cv::Vec4f& c,
           int len, int height, int scale, float threshold) {
  // first change the index to mat idx, a.k.a. scalex
  c_idx *= scale; r_idx *= scale;
  len *= scale; height *=scale;

  std::vector<std::pair<float, float>> idx_vals = {};
  std::vector<float> range_vals = {};

  filter_vals(img, idx_vals, range_vals, c_idx, r_idx, len, height);

  // this means all points are zeros;
  if (idx_vals.size() == 0) {
    c = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
    return true;
  }

  // plane fitting
  try {
    c = plane_fitting(idx_vals, range_vals);
  } catch (int e) {
    return false;
  }

  if (!check_mat(img, c, threshold, c_idx, r_idx, len, height)) {
    // comp_result(img, comp_mat, c, threshold, c_idx, r_idx, len, height, idx);
    return false;
  }

  return true;
}


/*
 * img: the range image from the orignal point cloud
 * b_mat: the binary map to indicate whether the tile is fitted or not
 * idx_size: the dimension of the point cloud divided by the tile size
 * NOTE: img size is [tile_size] larger than b_mat
 * */
void single_channel_fit(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
     std::vector<cv::Vec4f>& coefficients, std::vector<int>& tile_fit_lengths,
     const float threshold, const int tile_size) {

  int fit_cnt = 0, unfit_cnt = 0;
  int tt2 = tile_size*tile_size;
  // first assume that every kxk tile can be fitted into a plane;
  // here both i and j and len are in the unit of kxk tile.
  // the initial step is to merge kxk tile horizontally.
  for (int r_idx = 0; r_idx < idx_sizes[0]; r_idx++) {
    int c_idx = 0;
    int serial_num = 0;
    int len = 1;
    cv::Vec4f prev_c(0.f, 0.f, 0.f, 0.f);
    cv::Vec4f c(0.f, 0.f, 0.f, 0.f);

    while (c_idx+len < idx_sizes[1]) {
      // when (len==1) save c to prev_c in case len can't be 2
      if (len == 1) {
        if (!merge(img, c_idx, r_idx, prev_c, 1, 1, tile_size, threshold)) {
          // set the b_mat to be 0 indicated the tile cannot be fitted
          b_mat.at<int>(r_idx, c_idx) = 0;
          c_idx++;
          len = 1;
          unfit_cnt++;
          continue;
        } else {
          fit_cnt++;
          // tile can be fitted
          b_mat.at<int>(r_idx, c_idx) = 1;
        }
      }

      if (merge(img, c_idx, r_idx, c, len+1, 1, tile_size, threshold)) {
        // if len+c_idx already greater than the entire column size
        if (len+c_idx >= idx_sizes[1]) {
          coefficients.push_back(cv::Vec4f(c));
          tile_fit_lengths.push_back(std::min(len+1, idx_sizes[1]-c_idx));
          b_mat.at<int>(r_idx, idx_sizes[1]-1) = 1;
          break;
        } else {
          prev_c = cv::Vec4f(c);
          b_mat.at<int>(r_idx, c_idx+len) = 1;
        }
        len++;
        fit_cnt++;
      } else {
        coefficients.push_back(cv::Vec4f(prev_c));
        tile_fit_lengths.push_back(len);
        c_idx = c_idx+len;
        len = 1;
        prev_c = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
        unfit_cnt++;
        if (len+c_idx >= idx_sizes[1]) {
          break;
        }
      }
    }
    coefficients.push_back(cv::Vec4f(prev_c));
  }

  std::cout << "Single with fitting_cnts: " << fit_cnt
            << " with unfitting_cnts: " << unfit_cnt << std::endl;
  return;
}

/*
// this function is the initialization
// of the oracle analysis on one particular
// function.
std::pair<float, cv::Vec4f> multi_channel_compression(std::vector<cv::Mat*>& img, 
          std::vector<cv::Mat*>& r_img, const int * mat_sizes, float threshold,
          int tile_size, int channel, cv::Mat& factor_m) {

  // tile times tile
  int tt2 = tile_size*tile_size;
  
  //  CROSS CHANNEL FITTING
  //
  int idx_sizes[] = {mat_sizes[0], mat_sizes[1]/tile_size, mat_sizes[2]/tile_size};
  // some init
  int cross_fit_cnt = 0;
  float cross_point_cnt = 0.f;

  if (channel > 1) {
    auto cross_fit_res =  cross_channel_fit(img, r_img, idx_sizes, threshold, tile_size, channel);
    cross_fit_cnt = cross_fit_res.first;
    cross_point_cnt = cross_fit_res.second;
  }
  std::cout << "Cross-channel fitting with cross_fit_cnts: " << cross_fit_cnt << std::endl;
  float cross_fitting_ratio = cross_point_cnt/(mat_sizes[1]*mat_sizes[2]*mat_sizes[0]);
  std::cout << "Cross-channel filling ratio: " << cross_fitting_ratio << std::endl;

  float indi_point_cnt = 0.f;
  std::vector<int> fit_cnts(channel, 0);
  std::vector<int> unfit_cnts(channel, 0);
  // use to record the comparison between actual values and computed values
  cv::Mat comp_mat(3, mat_sizes, CV_32FC2, cv::Scalar(0, 0));
  // construct a index map
  cv::Mat idx_mats(3, idx_sizes, CV_32SC1, cv::Scalar(0));
  
  indi_point_cnt = single_channel_fit(img, r_img, idx_sizes, threshold, comp_mat,
                                      idx_mats, tile_size, channel, fit_cnts, unfit_cnts);

  float individual_fitting_ratio = indi_point_cnt/(mat_sizes[1]*mat_sizes[2]*mat_sizes[0]);
  std::cout << "Individual-channel filling ratio: " << individual_fitting_ratio << std::endl;
  
  float median_fit_cnt = median_check(img, r_img, comp_mat, threshold, mat_sizes, tile_size);
  std::cout << "Median fitting_cnts: " << median_fit_cnt << std::endl;
  float median_fitting_ratio = median_fit_cnt*tt2/(mat_sizes[1]*mat_sizes[2]*mat_sizes[0]);
  std::cout << "Median filling ratio: " << median_fitting_ratio << std::endl;
  
  float unfitting_cnt = copy_unfit_points(img, r_img, mat_sizes, tile_size);
  float unfitting_ratio = unfitting_cnt*tt2/(mat_sizes[1]*mat_sizes[2]*mat_sizes[0]);
  std::cout << "Unfilling ratio: " << unfitting_ratio << std::endl;

  int delta_cnt = delta_coding(comp_mat, mat_sizes, tile_size);
  //unfitting_cnt = export_comp(comp_mat, "comp_mat.csv", mat_sizes, 2);

  float tile_fit_cnt = 0.f;
  for (auto i : fit_cnts)
    tile_fit_cnt += i;

  float cnt_bytes = (2.0f+idx_sizes[0])*cross_fit_cnt + 4.0f*median_fit_cnt 
                  + 3.0f*tile_fit_cnt + delta_cnt;

  // this is the additional bytes for fit coefficients index
  cnt_bytes += (2.0f * (cross_fit_cnt + tile_fit_cnt) + 2.0f/8.0f*idx_sizes[1]*idx_sizes[2]);

  float compression_rate = cnt_bytes / (mat_sizes[1]*mat_sizes[2]*channel);

#ifndef PERFORMANCE
  std::cout << "Compression rate: " << compression_rate
            << "\n\twith cross fit: " << cross_fit_cnt
            << " with tile fit: " << tile_fit_cnt
            << "\n\twith unfit: " << unfitting_cnt << std::endl;
#endif

  // return std::make_pair(0, 0);
  return std::make_pair(compression_rate,
                        cv::Vec4f(cross_fitting_ratio,
                                  individual_fitting_ratio,
                                  median_fitting_ratio,
                                  unfitting_ratio));
} */

}
