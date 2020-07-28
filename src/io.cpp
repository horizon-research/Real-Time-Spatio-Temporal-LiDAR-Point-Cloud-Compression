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
  out_stream.close();
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
  in_stream.close();
}

void export_coefficients(std::vector<cv::Vec4f>& coefficients,
                         std::string filename) {
  std::ofstream out_stream(filename, std::ofstream::binary);

  for (auto c : coefficients) {
    out_stream.write(reinterpret_cast<const char*>(&c[0]), sizeof(c[0]));
    out_stream.write(reinterpret_cast<const char*>(&c[1]), sizeof(c[1]));
    out_stream.write(reinterpret_cast<const char*>(&c[2]), sizeof(c[2]));
    out_stream.write(reinterpret_cast<const char*>(&c[3]), sizeof(c[3]));
  }

  out_stream.close();
}

void import_coefficients(std::vector<cv::Vec4f>& coefficients,
                         std::string filename) {
  std::ifstream in_stream(filename, std::ifstream::binary);
  float c[4];
  while (in_stream.read(reinterpret_cast<char*>(c), sizeof(float)*4)) {
    coefficients.push_back(cv::Vec4f(c[0], c[1], c[2], c[3]));
  }

  in_stream.close();
}

void export_occ_mat(cv::Mat& occ_mat, std::string filename) {
  // open a file
  std::ofstream out_stream(filename, std::ofstream::binary);

  unsigned short code;
  for (int row = 0; row < occ_mat.rows; row++) {
    for (int col = 0; col < occ_mat.cols; col++) {
      code = (unsigned short)occ_mat.at<int>(row, col);
      out_stream.write(reinterpret_cast<const char*>(&code), sizeof(code));
    }
  }
  out_stream.close();
}

void import_occ_mat(cv::Mat& occ_mat, std::string filename) {
  // open a file
  std::ifstream in_stream(filename, std::ifstream::binary);

  unsigned short code = 0;
  for (int row = 0; row < occ_mat.rows; row++) {
    for (int col = 0; col < occ_mat.cols; col++) {
      in_stream.read(reinterpret_cast<char*>(&code), sizeof(code));
      occ_mat.at<int>(row, col) = code;
    }
  }
  in_stream.close();
}

void export_unfit_nums(std::vector<float>& data, std::string filename) {
  std::ofstream out_stream(filename, std::ofstream::binary);

  for (auto d : data) {
    unsigned short quantized_d = (unsigned short) (d*256);
    out_stream.write(reinterpret_cast<const char*>(&quantized_d), sizeof(quantized_d));
  }
  out_stream.close();
}

void import_unfit_nums(std::vector<float>& data, std::string filename) {
  std::ifstream in_stream(filename, std::ifstream::binary);
  unsigned short quantized_d;
  while (in_stream.read(reinterpret_cast<char*>(&quantized_d), sizeof(quantized_d))) {
    float d = ((float) quantized_d) / 256.0f;
    data.push_back(d);
  }
  in_stream.close();
}

void export_tile_fit_lengths(std::vector<int>& data, std::string filename) {
  std::ofstream out_stream(filename, std::ofstream::binary);

  for (auto d : data) {
    unsigned short quantized_d = (unsigned short) (d);
    out_stream.write(reinterpret_cast<const char*>(&quantized_d), sizeof(quantized_d));
  }
  out_stream.close();
}

void import_tile_fit_lengths(std::vector<int>& data, std::string filename) {
  std::ifstream in_stream(filename, std::ifstream::binary);
  unsigned short quantized_d;
  while (in_stream.read(reinterpret_cast<char*>(&quantized_d), sizeof(quantized_d))) {
    data.push_back(quantized_d);
  }
  in_stream.close();
}

void export_plane_offsets(std::vector<std::vector<float>>& data, std::string filename) {
  std::ofstream out_stream(filename, std::ofstream::binary);

  for (auto vec : data) {
    for (auto d : vec) {
      out_stream.write(reinterpret_cast<const char*>(&d), sizeof(d));
    }
  }
  out_stream.close();
}

void import_plane_offsets(std::vector<std::vector<float>>& data,
                          std::string filename, int size) {
  std::ifstream in_stream(filename, std::ifstream::binary);
  std::vector<float> vec;
  float d;
  while (in_stream.read(reinterpret_cast<char*>(&d), sizeof(d))) {
    vec.push_back(d);
    if (vec.size() == size) {
      data.push_back(std::vector<float>(vec));
      vec.clear();
    }
  }
  in_stream.close();
}

void export_filenames(const std::vector<std::string>& data, std::string filename) {
  std::ofstream out_stream(filename);
 
  for (auto str : data)
    out_stream << str << std::endl;

  out_stream.close();
}


void import_filenames(std::vector<std::string>& data, std::string filename) {
  std::ifstream in_stream(filename);

  std::string str;
  while (getline(in_stream, str))
    data.push_back(str);

  in_stream.close();
}


