
template <class T>
void pcloud_to_mat(std::vector<point_cloud>& pcloud_data, cv::Mat& img,
                   float pitch_precision, float yaw_precision) {
  /* x, y, z coordinates. */
  float x, y, z;

  for (auto point : pcloud_data) {
    x = point.x;
    y = point.y;
    z = point.z;

    /* calculate some parameters for spherical coord */
    float dist = sqrt(x*x + y*y);
    float radius = sqrt(x*x + y*y + z*z);
    float pitch = atan2(y, x) * 180.0f / pitch_precision / PI;
    float yaw = atan2(z, dist) * 180.0f / yaw_precision / PI;

    float row_offset = ROW_OFFSET/yaw_precision;
    float col_offset = COL_OFFSET/pitch_precision;

    int col = std::min(img.cols-1, std::max(0, (int)(pitch+col_offset)));
    int row = std::min(img.rows-1, std::max(0, (int)(yaw+row_offset)));

    if (std::is_same<T, cv::Vec4f>::value) {
      img.at<cv::Vec4f>(row, col) = cv::Vec4f(radius, x, y, z);
    } else {
      img.at<T>(row, col) = radius;
    }
  }
}

template <class T>
void export_mat(cv::Mat& mat, const char* file_name, int precision) {
  std::ofstream outfile;
  outfile << std::fixed;
  outfile.open(file_name);
  for (int i = 0; i < mat.cols; i++) {
    for (int j = 0; j < mat.rows; j++) {
      if (std::is_same<T, cv::Vec4f>::value) {
        auto vec = mat.at<cv::Vec4f>(j, i);
        outfile << std::setprecision(precision)
                << vec[0]
                << ":" << vec[1] // <<":"<< vec[2] <<":"<< vec[3]
                <<  ",";
      } else {
        outfile << std::setprecision(precision) << mat.at<T>(j,i) <<  ",";
      }
    }
    outfile << std::endl;
  }
  outfile.close();
}

