#include "utils.h"

namespace decoder
{

/*
 * copy a tile into reconstructed range image
 * */
int copy_unfit_points(cv::Mat& img, std::vector<float>& unfit_nums,
                      int unfit_nums_itr, int occ_code,
                      int r_idx, int c_idx, int tile_size) {
  int itr = 0;
  for (int row = r_idx*tile_size; row < (r_idx+1)*tile_size; row++) {
    for (int col = c_idx*tile_size; col < (c_idx+1)*tile_size; col++) {
      if (((occ_code>>itr)&1) == 1) {
        img.at<float>(row, col) = unfit_nums[unfit_nums_itr];
        unfit_nums_itr++;
      }
      itr++;
    }
  }
  return unfit_nums_itr;
}

void calc_fit_nums(cv::Mat& img, const cv::Vec4f& c, int occ_code, int c_idx,
                   int r_idx, int len_itr, int tile_size) {
  int itr = 0;
  for (int j = 0; j < tile_size; j++) {
    for (int i = 0; i < tile_size; i++) {
      if (((occ_code>>itr)&1) == 1) {
        float val = fabs((c[3] + c[1]*j + c[2]*(len_itr*tile_size+i))/c[0]);
        if (!std::isnan(val))
          img.at<float>(r_idx*tile_size+j, c_idx*tile_size+i) = val;
      }
      itr++;
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
double single_channel_decode(cv::Mat& img, cv::Mat& b_mat, const int* idx_sizes,
                             std::vector<cv::Vec4f>& coefficients, cv::Mat& occ_mat,
                             std::vector<int>& tile_fit_lengths,
                             std::vector<float>& unfit_nums, int tile_size,
                             cv::Mat* multi_mat = nullptr) {

  auto decode_start = std::chrono::high_resolution_clock::now();

  int tt2 = tile_size*tile_size;
  int fit_itr = 0;
  int unfit_nums_itr = 0;

  int unfit_cnt = 0;
  int fit_cnt = 0;
  // first assume that every kxk tile can be fitted into a plane;
  // here both i and j and len are in the unit of kxk tile.
  // the initial step is to merge kxk tile horizontally.
  for (int r_idx = 0; r_idx < idx_sizes[0]; r_idx++) {
    int len_itr = 0;
    int len = 0;
    cv::Vec4f c(0.f, 0.f, 0.f, 0.f);
    for (int c_idx = 0; c_idx < idx_sizes[1]; c_idx++) {
      int tile_status = b_mat.at<int>(r_idx, c_idx);
      int occ_code = occ_mat.at<int>(r_idx, c_idx);
      // if 0, copy unfitfted nums to range image
      if (tile_status == 0) {
        if (len_itr < len) {
          std::cout << "[ERROR]: should encode unfit nums right now!" << std::endl;
          std::cout << "[INFO]: r_idx " << r_idx << " c_idx " << c_idx << " len_itr " 
                    << len_itr << " len " << len << std::endl;
          exit(0);
        }
        unfit_nums_itr = copy_unfit_points(img, unfit_nums, unfit_nums_itr,
                                           occ_code, r_idx, c_idx, tile_size);
        unfit_cnt++;
      } else {
        if (len_itr < len) {
          if (multi_mat == nullptr || multi_mat->at<int>(r_idx, c_idx) == 0)
            calc_fit_nums(img, c, occ_code, c_idx, r_idx, len_itr, tile_size);
          
          len_itr++;
        } else {
          c = coefficients[fit_itr];
          len_itr = 0;
          len = tile_fit_lengths[fit_itr];

          if (multi_mat == nullptr || multi_mat->at<int>(r_idx, c_idx) == 0)
            calc_fit_nums(img, c, occ_code, c_idx, r_idx, len_itr, tile_size);
          
          fit_itr++;
          len_itr++;
        }
        fit_cnt++;
      }
    }
  }

  auto decode_end = std::chrono::high_resolution_clock::now();
  double decode_time = std::chrono::duration_cast<std::chrono::nanoseconds>(decode_end-decode_start).count();  
  decode_time *= 1e-9;

  std::cout << "Single with fitting_cnts: " << fit_cnt
            << " with unfitting_cnts: " << unfit_cnt << std::endl;

  return decode_time;
}

void calc_fit_nums_w_offset(cv::Mat& img, const cv::Vec4f& c, int occ_code, int c_idx,
                            int r_idx, int len_itr, int tile_size, float offset) {
  int itr = 0;
  for (int j = 0; j < tile_size; j++) {
    for (int i = 0; i < tile_size; i++) {
      if (((occ_code>>itr)&1) == 1) {
        float val = fabs((offset + c[1]*j + c[2]*(len_itr*tile_size+i))/c[0]);
        img.at<float>(r_idx*tile_size+j, c_idx*tile_size+i) = val;
      }
      itr++;
    }
  }
  return;
}

void multi_channel_decode(std:: vector<cv::Mat*>& imgs, cv::Mat& b_mat, 
                            const int* idx_sizes,
                            const std::vector<cv::Mat*>& occ_mats,
                            std::vector<cv::Vec4f>& coefficients,
                            std::vector<std::vector<float>>& plane_offsets,
                            std::vector<int>& tile_fit_lengths, 
                            const float threshold, const int tile_size) {
 
  int tt2 = tile_size*tile_size;
 
  int unfit_cnt = 0;
  int fit_cnt = 0;

  for (int ch = 0; ch < imgs.size(); ch++) {
    std::cout << "[CHANNEL] " << ch << std::endl;
    int fit_itr = 0;
    for (int r_idx = 0; r_idx < idx_sizes[0]; r_idx++) {
      int len_itr = 0;
      int len = 0;
      cv::Vec4f c(0.f, 0.f, 0.f, 0.f);
      std::vector<float> offsets = std::vector<float>(plane_offsets[fit_itr]);
  
      for (int c_idx = 0; c_idx < idx_sizes[1]; c_idx++) {
        // std::cout << "tile: " << r_idx << ", " << c_idx << std::endl;
        int tile_status = b_mat.at<int>(r_idx, c_idx);
        int occ_code = occ_mats[ch]->at<int>(r_idx, c_idx);

        if (tile_status == 0) {
          if (len_itr < len) {
            std::cout << "[ERROR]: should encode unfit nums right now!" << std::endl;
            std::cout << "[INFO]: r_idx " << r_idx << " c_idx " << c_idx << " len_itr " 
                      << len_itr << " len " << len << std::endl;
            exit(0);
          }
          unfit_cnt++;
        } else {
          if (len_itr < len) {
            calc_fit_nums_w_offset(*(imgs[ch]), c, occ_code, c_idx, r_idx,
                                   len_itr, tile_size, offsets[ch]);
            len_itr++;
          } else {
            c = coefficients[fit_itr];
            offsets = std::vector<float>(plane_offsets[fit_itr]);
            len_itr = 0;
            len = tile_fit_lengths[fit_itr];
            calc_fit_nums_w_offset(*(imgs[ch]), c, occ_code, c_idx, r_idx,
                                   len_itr, tile_size, offsets[ch]);
            fit_itr++;
            len_itr++;
          }
          fit_cnt++;
        }
      }
    }
  }

  std::cout << "Multi with fitting_cnts: " << fit_cnt
            << " with unfitting_cnts: " << unfit_cnt << std::endl;
}

}
