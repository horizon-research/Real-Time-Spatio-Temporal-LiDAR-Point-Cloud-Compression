
template <class T>
T neighbor_search(T radius, cv::Mat& img, int row, int col) {
  T restored_radius = img.at<T>(row, col);
  
  for (int i = std::max(0,row-1); i < std::min(img.rows, row+2); i++) {
    for (int j = std::max(0,col-1); j < std::min(img.cols, col+2); j++) {
      if (std::abs(restored_radius-radius) > std::abs(img.at<T>(i, j)-radius))
        restored_radius = img.at<T>(i, j);
    }
  }
  return restored_radius;
}

template <class T>
float neighbor_search_vec(float radius, cv::Mat& img, int row, int col) {
  float restored_radius = img.at<T>(row, col)[0];
  
  for (int i = std::max(0,row-1); i < std::min(img.rows, row+2); i++) {
    for (int j = std::max(0,col-1); j < std::min(img.cols, col+2); j++) {
      if (fabs(restored_radius-radius) > fabs(img.at<T>(i, j)[0]-radius))
        restored_radius = img.at<T>(i, j)[0];
    }
  }
  return restored_radius;
}

template <class T>
float compute_loss_rate(cv::Mat& img, const std::vector<point_cloud>& pcloud,
                        float pitch_precision, float yaw_precision) {
  
  // initialize different error metric;
  float x_error = 0.0f, y_error = 0.0f, z_error = 0.0f;
  float error = 0.0f, dist_error = 0.0f;
  float norm_error = 0.0f, norm_dist_error = 0.0f;
  float max_radius = 0.0f, min_radius = 0.0f;
  
  for (auto point : pcloud) {
    float x = point.x;
    float y = point.y;
    float z = point.z;

		/* calculate some parameters for spherical coord */
	  float dist = sqrt(x*x + y*y);
	  float radius = sqrt(x*x + y*y + z*z);
	  float pitch = atan2(y, x) * 180.0f / pitch_precision / PI;     
	  float yaw = atan2(z, dist) * 180.0f / yaw_precision / PI;

    float row_offset = ROW_OFFSET/yaw_precision;
    float col_offset = COL_OFFSET/pitch_precision;
    
    int col = std::min(img.cols-1, std::max(0, (int)(pitch+col_offset)));
    int row = std::min(img.rows-1, std::max(0, (int)(yaw+row_offset)));
    
    // find max and min
    max_radius = fmax(max_radius, radius);
    min_radius = fmin(min_radius, radius);

    float restored_radius(0);
    if (std::is_same<T, cv::Vec4f>::value) {
      // restored_radius = img.at<cv::Vec4f>(row, col)[0];
      restored_radius = neighbor_search_vec<cv::Vec4f>(radius, img, row, col);
    } else {
      // radius distance
      // restored_radius = img.at<float>(row, col);
      restored_radius = neighbor_search<float>(radius, img, row, col);
    }

    if (std::isnan(restored_radius) || (restored_radius > 150.f) || restored_radius < 0.1f) {
      restored_radius = 0.1f;
      std::cout << "[IGNORE]: " << radius << ". [x,y,z]: " <<  x << ", " << y << ", " << z << std::endl;
    }

    float restored_x, restored_y, restored_z;
    compute_cartesian_coord(restored_radius, yaw, pitch,
                            restored_x, restored_y, restored_z,
                            pitch_precision, yaw_precision);
    
    compute_cartesian_coord(restored_radius,
        (row+0.5f)-row_offset, (col+0.5f)-col_offset,
        restored_x, restored_y, restored_z,
        pitch_precision, yaw_precision);

    double x_diff = fabs(x - restored_x);
    double y_diff = fabs(y - restored_y); 
    double z_diff = fabs(z - restored_z);
    double diff = sqrt(x_diff*x_diff + y_diff*y_diff + z_diff*z_diff);

    x_error += x_diff;
    y_error += y_diff;
    z_error += z_diff;
    error += diff;

    norm_error += diff / radius;
    dist_error += abs(restored_radius - radius);
    norm_dist_error += abs(restored_radius - radius) / radius;
  }

  if (VERBOSE) {
    std::cout << "the x-error rate: " << x_error / pcloud.size() << std::endl;
    std::cout << "the y-error rate: " << y_error / pcloud.size() << std::endl;
    std::cout << "the z-error rate: " << z_error / pcloud.size() << std::endl;
    std::cout << "the error rate: " << error / pcloud.size() << std::endl;
    std::cout << "the normal error rate: " << norm_error / pcloud.size() << std::endl;
    std::cout << "the distance error rate: " << dist_error / pcloud.size() << std::endl;
    std::cout << "the normal distance error rate: " << norm_dist_error / pcloud.size() << std::endl;
  }

  // find the maximum boundary among x, y, z;
  double bb_width = 2*max_radius;
  // divide the error by the number of points
  error = error / pcloud.size();

  // compute peak-signal-to-noise ratio (psnr) 
  return 10.0*log10((bb_width*bb_width)/(error*error));
}

template <class T>
void characterize_occlusion(cv::Mat& img, uint32_t total_points) {
  int empty_slots = 0;
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      uint8_t val = img.at<T>(i, j);
      if (val == 0)
        empty_slots++;
    }
  }
  int missed_points = total_points - ( img.rows * img.cols - empty_slots);
  if (VERBOSE)
    std::cout << "Total points: " << total_points
      << "; missed_points: " << missed_points
      << "; empty_slots: " << empty_slots << std::endl;
}

template <class T>
float characterize_neighbors(cv::Mat& img) {
  std::vector<float> diff = {};
  // iterate the map
  for (int i = 0; i < img.rows-1; i++) {
    for (int j = 0; j < img.cols-1; j++) {
      if (img.at<T>(i, j) != 0 && img.at<T>(i+1, j) != 0)  {
        diff.push_back(fabs(img.at<T>(i, j)-img.at<T>(i+1, j)));
      }
      if (img.at<T>(i, j) != 0 && img.at<T>(i, j+1) != 0)  {
        diff.push_back(fabs(img.at<T>(i, j)-img.at<T>(i, j+1)));
      }
    }
  }

  float sum = std::accumulate(diff.begin(), diff.end(), 0);

  std::vector<float> threshold = {0.01, 0.05, 0.1, 0.25, 1.0, 100};
  std::map<float, int> m;

  if (VERBOSE) {
    for (auto th : threshold)
      m[th] = 0;


    for (auto v : diff) {
      for (auto th : threshold) {
        if (v <= th) {
          m[th]++;
          break;
        }
      }
    }
    for (auto th : threshold) {
      std::cout << "Map["<< th <<"]" << " : " << m[th] << std::endl;
    }
  }
  return sum / diff.size();
}
