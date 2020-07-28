/*
 * This is an example of 3d lidar compression.
 * */

#include <stdlib.h>
#include "pcc_module.h"
#include "encoder.h"
#include "decoder.h"
#include "io.h"

int main(int argc, char** argv) { 
  
  std::string file_path, file_name, out_file;
  std::string input_format("binary");
  float pitch_precision, yaw_precision, threshold;
  int tile_size;

  namespace po = boost::program_options;

  po::options_description opts("PCC options");
  opts.add_options()
    ("help,h", "Print help messages")
    ("path", po::value<std::string>(&file_path)->required(), "raw point cloud data path")
    ("file", po::value<std::string>(&file_name)->required(), "raw point cloud data path")
    ("out", po::value<std::string>(&out_file)->required(), "compressed data filename")
    ("pitch,p", po::value<float>(&pitch_precision)->required(), "pitch precision")
    ("yaw,y", po::value<float>(&yaw_precision)->required(), "yaw precision")
    ("threshold,t", po::value<float>(&threshold)->required(), "threshold value for fitting")
    ("format,f",  po::value<std::string>(&input_format),
     "trace_file input format: binary(default) or ascii")
    ("tile,l", po::value<int>(&tile_size)->required(), "fitting tile size");

  po::variables_map vm;

  try 
  {
    po::store(po::parse_command_line(argc, argv, opts), vm);
    
    if (vm.count("help")) 
    {
      std::cout << "Point Cloud Compression" << std::endl 
        << opts << std::endl;
      return 0;
    }

    po::notify(vm);
  } catch(po::error& e) { 
    std::cerr << "ERROR: " << e.what() << std::endl << std::endl; 
    std::cerr << opts << std::endl; 
    return -1;
  }
  
  // create a vector to store frames;
  std::vector<point_cloud> pcloud_data;
  file_path = file_path + "/" + file_name;
  load_pcloud(file_path, pcloud_data);

  std::vector<std::string> filenames;
  filenames.push_back(file_name);
  export_filenames(filenames, "filenames.bin");

  PccResult pcc_res;

  /*******************************************************************/
  // initialization

  int row = (VERTICAL_DEGREE/yaw_precision);
  row = ((row + tile_size-1)/tile_size)*tile_size;
  int col = HORIZONTAL_DEGREE/pitch_precision + tile_size;
  col = ((col + tile_size-1)/tile_size)*tile_size;

  double proj_time, fit_time;
  float psnr, total_pcloud_size;

  /*******************************************************************/
  // convert range map

  std::cout << "CURRENT pcloud size: " << pcloud_data.size() << std::endl;
  
  // Characterize Range Map
  // floating map;
  cv::Mat* f_mat = new cv::Mat(row, col, CV_32FC4, cv::Scalar(0.f,0.f,0.f,0.f));
  
  proj_time = map_projection(*f_mat, pcloud_data, pitch_precision, yaw_precision, 'e');
  
  pcc_res.proj_times->push_back(proj_time);
  
  // compute compression rate: bit-per-point (bpp)
  pcc_res.compression_rate->push_back(8.0f*f_mat->cols*f_mat->rows/pcloud_data.size());
  
  // loss error compute;
  //psnr = compute_loss_rate<cv::Vec4f>(*f_mat, pcloud_data, pitch_precision, yaw_precision);
  
  // update the info;
  pcc_res.loss_rate->push_back(psnr);
  
  /*******************************************************************/
  // fitting range map
  int mat_div_tile_sizes[] = {row/tile_size, col/tile_size};
  std::vector<cv::Vec4f> coefficients;
  std::vector<int> tile_fit_lengths;
  std::vector<float> unfit_nums;

  cv::Mat* b_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
  cv::Mat* occ_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
 
  // encode the occupatjon map  
  encoder::encode_occupation_mat(*f_mat, *occ_mat, tile_size, mat_div_tile_sizes);

  fit_time = encoder::single_channel_encode(*f_mat, *b_mat, mat_div_tile_sizes, coefficients, 
                                            unfit_nums, tile_fit_lengths,
                                            threshold, tile_size);
  delete f_mat;

  pcc_res.fit_times->push_back(fit_time);

  // what we need to store:
  // 1. b_mat: binary map for tile type
  export_b_mat(*b_mat, "b_mat.bin");
  delete b_mat;

  // 2. planar coefficients
  export_coefficients(coefficients, "coefficients.bin");
  coefficients.clear();
  
  // 3. occ_mat: occupation map
  export_occ_mat(*occ_mat, "occ_mat.bin");
  delete occ_mat;

  // 4. unfit_nums: unfitted_nums
  export_unfit_nums(unfit_nums, "unfit_nums.bin");
  unfit_nums.clear();
  
  // 5. tile_fit_lengths
  export_tile_fit_lengths(tile_fit_lengths, "tile_fit_lengths.bin");
  tile_fit_lengths.clear();
  
  // 6. make a tar.gz file
  std::string cmd;
  cmd = "tar -czvf" + out_file + " filenames.bin"  + " b_mat.bin"
      + " coefficients.bin" + " occ_mat.bin"
      + " unfit_nums.bin" + " tile_fit_lengths.bin";

  if (system(cmd.c_str()) == -1) {
    std::cout << "[ERROR]: 'tar' command executed failed." << std::endl;
    exit(-1);
  }
  
  std::string rm_cmd;
  cmd = rm_cmd + "rm " + " filenames.bin"  + " b_mat.bin"
      + " coefficients.bin" + " occ_mat.bin"
      + " unfit_nums.bin" + " tile_fit_lengths.bin";

  if (system(cmd.c_str()) == -1) {
    std::cout << "[ERROR]: 'rm' command executed failed." << std::endl;
    exit(-1);
  }
  return 0;
}

