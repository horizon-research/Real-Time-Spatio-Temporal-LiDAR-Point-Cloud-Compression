#include "io.h"

void export_b_mat(cv::Mat& b_mat, std::string filename) {
  // open a file
  std::ofstream out_stream(filename, std::ofstream::binary);

  int cnt = 0;
  char code = 0;
  for (int row = 0; row < b_mat.rows; row++) {
    for (int col = 0; col < b_mat.cols; col++) {
      if (cnt == 8) {
        out_stream.write(&code, sizeof(code));
        cnt = 0;
        code = 0;
      }
      int status = b_mat.at<int>(row, col);
      code += (status << cnt);
      cnt++;
    }
  }
  if (cnt > 0) {
    out_stream.write(&code, sizeof(code));
  }
}

void import_b_mat(cv::Mat& b_mat, std::string filename) {
  // open a file
  std::ifstream in_stream(filename, std::ifstream::binary);

  int cnt = 0;
  char code = 0;
  in_stream.read(&code, 1);
  for (int row = 0; row < b_mat.rows; row++) {
    for (int col = 0; col < b_mat.cols; col++) {
      if (cnt == 8) {
        in_stream.read(&code, sizeof(code));
        cnt = 0;
      }
      if (code>>cnt&1 == 1) {
        b_mat.at<int>(row, col) = 1;
      } else {
        b_mat.at<int>(row, col) = 0;
      }
      cnt++;
    }
  }
}
