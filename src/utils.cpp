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

void export_factor(cv::Mat& mat, const char* file_name, int precision) {
  std::ofstream outfile;
  outfile << std::fixed;
  outfile.open(file_name);
  for (int i = 0; i < mat.cols; i++) {
    for (int j = 0; j < mat.rows; j++) {
      const cv::Vec4f& vec = mat.at<cv::Vec4f>(j, i);
      outfile << std::setprecision(precision)
        << vec[0] << ":" << vec[1] <<":"<< vec[2] <<":"<< vec[3] <<  ",";
    }
    outfile << std::endl;
  }
  outfile.close();
}


void load_trace_data(const std::string& dir_path, std::vector<std::vector<point_cloud>>& frames,
    const std::string& input_format, int frame_cnt) {
  
  std::vector<std::string> trace_paths = {};
  /* iterate through the directory and add file names */
  for (auto itr : std::experimental::filesystem::directory_iterator(dir_path))
    trace_paths.push_back(itr.path());

  std::sort(trace_paths.begin(), trace_paths.end());
  // iterate the dataset directory;
  for ( auto curr_file : trace_paths ) {
    std::vector<point_cloud> pcloud_data = {};

    if (input_format.compare("binary") == 0) {
      // load point cloud data;
      load_pcloud(curr_file, pcloud_data);
    } else if (input_format.compare("ascii") == 0) {
      // load point cloud data;
      load_pcloud_xyz(curr_file, pcloud_data);
    } else {
      std::cout << "[ERROR]: can't read this input. " << std::endl;
      exit(-1);
    }

    std::cout << "Point cloud size: " << pcloud_data.size() << std::endl;
    
    frames.push_back(std::move(pcloud_data));
    if (frames.size() >= frame_cnt)
      break;
  }
}

void load_trace_calib(const std::string& calib_path,
    std::vector<std::vector<float>>& moves, int frame_cnt) {

  std::vector<std::string> calib_paths = {};
  /* iterate through the directory and add file names */
  for (auto itr : std::experimental::filesystem::directory_iterator(calib_path))
    calib_paths.push_back(itr.path());

  std::sort(calib_paths.begin(), calib_paths.end());

  // iterate the dataset directory;
  for (auto curr_file : calib_paths) {
    std::ifstream infile;
    infile.open(curr_file.c_str());

    std::vector<float> vec(25, 0.0f);
    for (int i = 0; i < vec.size(); i++)
      infile >> vec[i];
    
    infile.close();

    printf("velocity: %f %f %f\nangular speed: %f %f %f\n",
           vec[8], vec[9], vec[10], vec[20], vec[21], vec[22]);

    moves.push_back({vec[8], vec[9], vec[10], vec[20], vec[21], vec[22]});

    if (moves.size() >= frame_cnt)
      break;
  }
}

