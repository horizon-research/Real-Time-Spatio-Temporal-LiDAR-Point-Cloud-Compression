#include "encoder.h"

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
    std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 100};
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
 * copy a tile into unfitted number vector
 * */
void copy_unfit_points(cv::Mat& img, std::vector<float>& unfit_nums,
                       int r_idx,int c_idx, int tile_size) {

  for (int row = r_idx*tile_size; row < (r_idx+1)*tile_size; row++) {
    for (int col = c_idx*tile_size; col < (c_idx+1)*tile_size; col++) {
      float num = img.at<cv::Vec4f>(row, col)[0];
      if (num != 0.f) {
        unfit_nums.push_back(num);
      }
    }
  }
  return;
}

void encode_occupation_mat(cv::Mat& img, cv::Mat& occ_mat, int tile_size,
                           const int* idx_sizes) {

  for (int r_idx = 0; r_idx < idx_sizes[0]; r_idx++) {
    for (int c_idx = 0; c_idx < idx_sizes[1]; c_idx++) {
      int code = 0;
      for (int row = r_idx*tile_size; row < (r_idx+1)*tile_size; row++) {
        for (int col = c_idx*tile_size; col < (c_idx+1)*tile_size; col++) {
          float num = img.at<cv::Vec4f>(row, col)[0];
          if (num > 0.1f) {
            code += (1 << (row - r_idx*tile_size)*tile_size + (col - c_idx*tile_size));
          }
        }
      }
      occ_mat.at<int>(r_idx, c_idx) = code;
    }
  }
  return;
}

/*
 * img: the range image from the orignal point cloud
 * b_mat: the binary map to indicate whether the tile is fitted or not
 * idx_size: the dimension of the point cloud divided by the tile size
 * NOTE: img size is [tile_size] larger than b_mat
 * */
double single_channel_encode(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
                             std::vector<cv::Vec4f>& coefficients,
                             std::vector<float>& unfit_nums,
                             std::vector<int>& tile_fit_lengths, 
                             const float threshold, const int tile_size) {

  auto fit_start = std::chrono::high_resolution_clock::now(); 
  
  int fit_cnt = 0, unfit_cnt = 0;
  int tt2 = tile_size*tile_size;
  // first assume that every kxk tile can be fitted into a plane;
  // here both i and j and len are in the unit of kxk tile.
  // the initial step is to merge kxk tile horizontally.
  for (int r_idx = 0; r_idx < idx_sizes[0]; r_idx++) {
    int c_idx = 0;
    int len = 1;
    cv::Vec4f prev_c(0.f, 0.f, 0.f, 0.f);
    cv::Vec4f c(0.f, 0.f, 0.f, 0.f);

    while (c_idx+len <= idx_sizes[1]) {
      // when (len==1) save c to prev_c in case len can't be 2
      if (len == 1) {
        if (!merge(img, c_idx, r_idx, prev_c, 1, 1, tile_size, threshold)) {
          // set the b_mat to be 0 indicated the tile cannot be fitted
          b_mat.at<int>(r_idx, c_idx) = 0;
          c_idx++;
          len = 1;
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
        if (len+c_idx >= idx_sizes[1]) {
          break;
        }
      }
    }
    // copy all the unfitted numbers
    for (int c_idx = 0; c_idx < idx_sizes[1]; c_idx++) {
      if (b_mat.at<int>(r_idx, c_idx) == 0) {
        copy_unfit_points(img, unfit_nums, r_idx, c_idx, tile_size);
        unfit_cnt++;
      }
    }
  }

  auto fit_end = std::chrono::high_resolution_clock::now();
  double fit_time = std::chrono::duration_cast<std::chrono::nanoseconds>(fit_end-fit_start).count();  
  fit_time *= 1e-9;

  std::cout << "Single with fitting_cnts: " << fit_cnt
            << " with unfitting_cnts: " << unfit_cnt << std::endl;
  return fit_time;
}

bool test_tile(cv::Mat& img, const cv::Vec4f& c, float threshold,
               int c_idx, int r_idx, int len, int height,
               std::vector<float>& offsets) {

  if (c[0] == 0.f && c[1] == 0.f && c[2] == 0.f)
    return false;

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
  float offset = (cnt == 0) ? 0 : sum/cnt;
  
  for (int j = 0; j < height; j++) {
    for (int i = 0; i < len; i++) {
      auto vec = img.at<cv::Vec4f>(r_idx+j, c_idx+i);
      float actual_value = vec[0];
      if (actual_value > 0) {
        float val = fabs(-offset/(c[0]*vec[0] + c[1]*j + c[2]*i))*vec[0];
        float diff = fabs(val - actual_value);
        
        if (diff > threshold) return false;
      }
    }
  }
  offsets.push_back(-offset);
  return true;
}

/*
 * test and merge the next tiles with previous fitted coefficients 
 * */
bool multi_merge(std::vector<cv::Mat*>& imgs, int c_idx, int r_idx,
                 cv::Vec4f& c, std::vector<float>& offsets,
                 int len, int height, int scale, float threshold) {
  // first change the index to mat idx, a.k.a. scalex
  c_idx *= scale; r_idx *= scale;
  len *= scale; height *=scale;
  offsets.clear();

  std::vector<std::pair<float, float>> idx_vals = {};
  std::vector<float> range_vals = {};

  // find a channel that the numbers of points are enough
  for (int idx = 0; idx < imgs.size(); idx++) {
    idx_vals.clear();
    range_vals.clear();
    filter_vals(*(imgs[idx]), idx_vals, range_vals,
                c_idx, r_idx, len, height);
    if (idx_vals.size() >= 4)
      break;
  }

  // 
  if (idx_vals.size() < 4) {
    c = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
    offsets = std::vector<float>(imgs.size(), 0.f);
    return false;
  }

  // plane fitting
  try {
    c = plane_fitting(idx_vals, range_vals);
  } catch (int e) {
    c = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
    offsets = std::vector<float>(imgs.size(), 0.f);
    return false;
  }

  for (int idx = 0; idx < imgs.size(); idx++) {
    if (!test_tile(*(imgs[idx]), c, threshold, c_idx, r_idx,
                   len, height, offsets)) {
      c = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
      offsets = std::vector<float>(imgs.size(), 0.f);
      return false;
    }
  }
  return true;
}

/*
 * copy a tile into unfitted number vector
 * */
void remove_fit_points(cv::Mat& img, int r_idx,int c_idx, int tile_size) {

  for (int row = r_idx*tile_size; row < (r_idx+1)*tile_size; row++) {
    for (int col = c_idx*tile_size; col < (c_idx+1)*tile_size; col++) {
      img.at<cv::Vec4f>(row, col) = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
    }
  }
  return;
}

double multi_channel_encode(std:: vector<cv::Mat*>& imgs, cv::Mat& b_mat, 
                            const int* idx_sizes,
                            std::vector<cv::Vec4f>& coefficients,
                            std::vector<std::vector<float>>& plane_offsets,
                            std::vector<int>& tile_fit_lengths, 
                            const float threshold, const int tile_size) {

  auto fit_start = std::chrono::high_resolution_clock::now(); 
  
  int fit_cnt = 0, unfit_cnt = 0;
  int tt2 = tile_size*tile_size;
  // first assume that every kxk tile can be fitted into a plane;
  // here both i and j and len are in the unit of kxk tile.
  // the initial step is to merge kxk tile horizontally.
  for (int r_idx = 0; r_idx < idx_sizes[0]; r_idx++) {
    int c_idx = 0;
    int len = 1;
    cv::Vec4f prev_c(0.f, 0.f, 0.f, 0.f);
    cv::Vec4f c(0.f, 0.f, 0.f, 0.f);
    std::vector<float> offsets;
    std::vector<float> prev_offsets;

    while (c_idx+len <= idx_sizes[1]) {
      // when (len==1) save c to prev_c in case len can't be 2
      if (len == 1) {
        if (!multi_merge(imgs, c_idx, r_idx, prev_c, prev_offsets,
                         1, 1, tile_size, threshold)) {
          // set the b_mat to be 0 indicated the tile cannot be fitted
          b_mat.at<int>(r_idx, c_idx) = 0;
          c_idx++;
          len = 1;
          continue;
        } else {
          fit_cnt++;
          // tile can be fitted
          b_mat.at<int>(r_idx, c_idx) = 1;
        }
      }

      if (multi_merge(imgs, c_idx, r_idx, c, offsets,
                      len+1, 1, tile_size, threshold)) {
        // if len+c_idx already greater than the entire column size
        if (len+c_idx >= idx_sizes[1]) {
          coefficients.push_back(cv::Vec4f(c));
          plane_offsets.push_back(std::vector<float>(offsets));
          tile_fit_lengths.push_back(std::min(len+1, idx_sizes[1]-c_idx));
          b_mat.at<int>(r_idx, idx_sizes[1]-1) = 1;
          break;
        } else {
          prev_c = cv::Vec4f(c);
          prev_offsets = std::vector<float>(offsets);
          b_mat.at<int>(r_idx, c_idx+len) = 1;
        }
        len++;
        fit_cnt++;
      } else {
        coefficients.push_back(cv::Vec4f(prev_c));
        plane_offsets.push_back(std::vector<float>(prev_offsets));
        tile_fit_lengths.push_back(len);
        c_idx = c_idx+len;
        len = 1;
        prev_c = cv::Vec4f(0.f, 0.f, 0.f, 0.f);
        if (len+c_idx >= idx_sizes[1]) {
          break;
        }
      }
    }
    // remove all the fitted numbers
    for (int ch = 0; ch < imgs.size(); ch++) {
      for (int c_idx = 0; c_idx < idx_sizes[1]; c_idx++) {
        if (b_mat.at<int>(r_idx, c_idx) == 1) {
          remove_fit_points(*(imgs[ch]), r_idx, c_idx, tile_size);
        }
      }
    }
  }

  auto fit_end = std::chrono::high_resolution_clock::now();
  double fit_time = std::chrono::duration_cast<std::chrono::nanoseconds>(fit_end-fit_start).count();  
  fit_time *= 1e-9;

  std::cout << "Multi with fitting_cnts: " << fit_cnt
            << " with unfitting_cnts: " << unfit_cnt << std::endl;
  
  return fit_time;
}


}
