#include "utils.h"

// this is used to load xyz binary file
void load_pcloud(std::string& file_name, 
                 std::vector<point_cloud>& pcloud_data) {

  /* allocate buffer to store the data */
  int32_t num = 1000000;
  float * buf = (float *) malloc(num*sizeof(float));

	/* pointers */
	float *px = buf+0;
	float *py = buf+1;
	float *pz = buf+2;
	float *pr = buf+3;

	/* open file to read */
	FILE * stream = fopen (file_name.c_str(), "rb");
	std::cout << file_name.c_str() << std::endl;
  // std::ofstream outstream("output.txt");

	num = fread(buf, sizeof(float), num, stream)/4;
	
  for (int32_t i = 0; i < num; i++) {
		pcloud_data.push_back(point_cloud(std::move((*px)),
													std::move((*py)),
													std::move((*pz)),
													std::move(*pr)));

		/* update the new point addresses */
		px+=4; py+=4; pz+=4; pr+=4;

	}
  /* free the additional space */
  free(buf);

  // outstream.close();
  fclose(stream);
}

// this is used to load xyz binary file
void load_pcloud_xyz(std::string& file_name,
                     std::vector<point_cloud>& pcloud_data) {

	/* pointers */
	float x, y, z;

	/* open file to read */
  std::string line;
  std::ifstream stream(file_name.c_str());
	std::cout << file_name << std::endl;
	
  while (std::getline(stream, line)) {
    auto cnt =  sscanf(line.c_str(), "%f %f %f", &x, &y, &z);
    if (cnt < 3) {
      std::cout << "Invalid line: " + line << std::endl;
      continue;
    }
		pcloud_data.push_back(point_cloud(x, y, z, 0.0f));
	}
  stream.close();
}

void pcloud2bin(std::string filename, std::vector<point_cloud>& pcloud_data) {
  /* pointers */
  float x, y, z, r;
  r = 0;

  /* open file to read */
  std::ofstream out_stream(filename, std::ofstream::binary);
  std::cout << "Write " << filename << std::endl;

  for (auto p : pcloud_data) {
    x = p.x;
    y = p.y;
    z = p.z;
    r = p.r;
    out_stream.write(reinterpret_cast<const char*>(&x), sizeof(x));
    out_stream.write(reinterpret_cast<const char*>(&y), sizeof(y));
    out_stream.write(reinterpret_cast<const char*>(&z), sizeof(z));
    out_stream.write(reinterpret_cast<const char*>(&r), sizeof(r));
  }
  out_stream.close();
}

void compute_cartesian_coord(const float radius, const float yaw,
                             const float pitch, float& x, float& y, float& z,
                             float pitch_precision, float yaw_precision) {
  // start to convert spherical coordinates
  // into cartesian coordinates;
  float xy_radius = ((float) radius)*cos(yaw/180.0f*PI*yaw_precision);

  // compute the coordinates;
  z = ((float) radius)*sin(yaw/180.0f*PI*yaw_precision);
  x = xy_radius*cos(pitch/180.0f*PI*pitch_precision); 
  y = xy_radius*sin(pitch/180.0f*PI*pitch_precision);
}

std::string pcloud2string(std::vector<point_cloud>& pcloud_data) {
  std::string ret = "[";
  for (auto p : pcloud_data) {
    ret += p.to_string_xyz() + ",";
  }
  ret.pop_back();
  ret += "]";
  return ret;
}

void output_cloud(const std::vector<point_cloud>& pcloud, std::string file_name) {

  std::ofstream stream(file_name.c_str());

  stream << "ply\nformat ascii 1.0\n";
  stream << "element vertex " << pcloud.size() << std::endl;
  stream << "property float x\nproperty float y\nproperty float z\nend_header" << std::endl;
  
  for (auto point : pcloud) {
    float x = point.x;
    float y = point.y;
    float z = point.z;

    stream << x << " " << y << " " << z << std::endl;
  }
  stream.close();

}

std::vector<float>
output_normalize_cloud(const std::vector<point_cloud>& pcloud, std::string file_name) {

  float min_x = 0.f, max_x = 0.f, min_y = 0.f, max_y = 0.f, min_z = 0.f, max_z = 0.f;
  for (auto point : pcloud) {
    float x = point.x;
    float y = point.y;
    float z = point.z;

    min_x = (min_x > x) ? x : min_x;
    min_y = (min_y > y) ? y : min_y;
    min_z = (min_z > z) ? z : min_z;
    max_x = (max_x < x) ? x : max_x;
    max_y = (max_y < y) ? y : max_y;
    max_z = (max_z < z) ? z : max_z;
  }

  std::ofstream stream(file_name.c_str());

  stream << "ply\nformat ascii 1.0\n";
  stream << "element vertex " << pcloud.size() << std::endl;
  stream << "property float x\nproperty float y\nproperty float z\nend_header" << std::endl;
  
  for (auto point : pcloud) {
    int x = (int)((point.x - min_x)/(max_x - min_x)*255.f);
    int y = (int)((point.y - min_y)/(max_y - min_y)*255.f);
    int z = (int)((point.z - min_z)/(max_z - min_z)*255.f);

    stream << x << " " << y << " " << z << std::endl;
  }
  stream.close();

  return {min_x, max_x, min_y, max_y, min_z, max_z};
}

float neighbor_search(float radius, cv::Mat& img, int row, int col) {
  float restored_radius = img.at<float>(row, col);
  
  for (int i = std::max(0,row-1); i < std::min(img.rows, row+2); i++) {
    for (int j = std::max(0,col-1); j < std::min(img.cols, col+2); j++) {
      if (std::abs(restored_radius-radius) > std::abs(img.at<float>(i, j)-radius))
        restored_radius = img.at<float>(i, j);
    }
  }
  return restored_radius;
}

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
    restored_radius = neighbor_search(radius, img, row, col);

    if (std::isnan(restored_radius) || (restored_radius > 150.f) || restored_radius < 0.1f) {
      std::cout << "[IGNORE]: " << radius << " vs. "  <<  restored_radius << ". [x,y,z]: " <<  x << ", " << y << ", " << z << std::endl;
      restored_radius = 0.1f;
    }

    float restored_x, restored_y, restored_z;
    compute_cartesian_coord(restored_radius, (row+0.5f)-row_offset, (col+0.5f)-col_offset,
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


void restore_pcloud(cv::Mat& img, float pitch_precision, float yaw_precision,
                    std::vector<point_cloud>& restored_pcloud) {

  std::cout << pitch_precision << " : " << yaw_precision << std::endl;
  for (int row = 0; row < img.rows; row++) {
    for (int col = 0; col < img.cols; col++) {
      float radius = img.at<float>(row, col);
      if (radius <= 0) continue;
      
      float pitch = (col + 0.5) * pitch_precision - COL_OFFSET;
      float yaw = (row + 0.5) * yaw_precision - ROW_OFFSET;
      // std::cout << yaw << std::endl;

      float z = radius*sin(yaw * PI / 180.0f);
      float dist = radius*cos(yaw * PI / 180.0f);
      float y = dist*sin(pitch * PI / 180.0f);
      float x = dist*cos(pitch * PI / 180.0f);

      restored_pcloud.push_back(point_cloud(x, y, z, radius));
    }
  }
}
