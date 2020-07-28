/*
 * This is an example of 3d lidar compression.
 * */

#include <stdlib.h>
#include "pcc_module.h"
#include "encoder.h"
#include "decoder.h"
#include "io.h"

int main(int argc, char** argv) { 
  
  std::string input_dir, out_file;
  std::string input_format("binary");
  float pitch_precision, yaw_precision, threshold;
  int tile_size;

  namespace po = boost::program_options;

  po::options_description opts("PCC options");
  opts.add_options()
    ("help,h", "Print help messages")
    ("input-path", po::value<std::string>(&input_dir)->required(), "major data path")
    ("input-files", po::value<std::vector<std::string>>()->multitoken(), "raw point cloud data path")
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
  std::vector<std::vector<point_cloud>> pcloud_data_list;
  for (auto filename : vm["input-files"].as<std::vector<std::string>>()) {
    std::string file_path = input_dir + "/" + filename;
    std::vector<point_cloud> pcloud_data;
    load_pcloud(file_path, pcloud_data);
    pcloud_data_list.push_back(pcloud_data);
  }

  PccResult pcc_res;

  /*******************************************************************/
  // initialization

  int row = (VERTICAL_DEGREE/yaw_precision);
  row = ((row + tile_size-1)/tile_size)*tile_size;
  int col = HORIZONTAL_DEGREE/pitch_precision + tile_size;
  col = ((col + tile_size-1)/tile_size)*tile_size;

  double proj_time, fit_time;
  float psnr, total_pcloud_size;
  int mat_div_tile_sizes[] = {row/tile_size, col/tile_size};

  /*******************************************************************/
  // convert range map
  std::vector<cv::Mat*> f_mats;
  for (auto pcloud_data : pcloud_data_list) {
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
    f_mats.push_back(f_mat); 
  }

  /*******************************************************************/
  // fit occupation maps
  std::vector<cv::Mat*> occ_mats;
  for (int i = 0; i < pcloud_data_list.size(); i++) {
    auto f_mat = f_mats[i];
    cv::Mat* occ_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
    // encode the occupatjon map
    encoder::encode_occupation_mat(*f_mat, *occ_mat, tile_size, mat_div_tile_sizes);
    occ_mats.push_back(occ_mat);
  }
  // store the coefficients for mutli-plane fitting
  std::vector<cv::Vec4f> multi_coefficients;
  std::vector<std::vector<float>> plane_offsets;
  std::vector<int> multi_tile_fit_lengths;
  cv::Mat*  multi_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);

  fit_time = encoder::multi_channel_encode(f_mats, *multi_mat, mat_div_tile_sizes,
                                           multi_coefficients, plane_offsets,
                                           multi_tile_fit_lengths, threshold,
                                           tile_size);

  std::cout << "multi_coefficients: " << multi_coefficients.size()
            << " plane_offsets: " << plane_offsets.size()
            << " multi_tile_len: " << multi_tile_fit_lengths.size() << std::endl;

  // command for making a tar.gz file
  std::string cmd = "tar -czvf " + out_file;
  std::string file_string;
 
  // output filenames
  export_filenames(vm["input-files"].as<std::vector<std::string>>(), "filenames.bin");
  file_string += " filenames.bin";

  for (int i = 0; i < pcloud_data_list.size(); i++) {
    auto pcloud_data = pcloud_data_list[i];
    auto f_mat = f_mats[i];
    cv::Mat* occ_mat = occ_mats[i];

    /*******************************************************************/
    // fitting range map
    std::vector<cv::Vec4f> coefficients;
    std::vector<int> tile_fit_lengths;
    std::vector<float> unfit_nums;
  
    cv::Mat* b_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);

    fit_time = encoder::single_channel_encode(*f_mat, *b_mat, mat_div_tile_sizes, coefficients, 
                                              unfit_nums, tile_fit_lengths,
                                              threshold, tile_size);
    delete f_mat;
  
    pcc_res.fit_times->push_back(fit_time);
  
    // what we need to store:
    // 1. b_mat: binary map for tile type
    export_b_mat(*b_mat, "b_mat"+std::to_string(i)+".bin");
    delete b_mat;
    file_string +=  " b_mat"+std::to_string(i)+".bin";
  
    // 2. planar coefficients
    export_coefficients(coefficients, "coefficients"+std::to_string(i)+".bin");
    coefficients.clear();
    file_string += " coefficients"+std::to_string(i)+".bin";
    
    // 3. occ_mat: occupation map
    export_occ_mat(*occ_mat, "occ_mat"+std::to_string(i)+".bin");
    delete occ_mat;
    file_string += " occ_mat"+std::to_string(i)+".bin";
  
    // 4. unfit_nums: unfitted_nums
    export_unfit_nums(unfit_nums, "unfit_nums"+std::to_string(i)+".bin");
    unfit_nums.clear();
    file_string += " unfit_nums"+std::to_string(i)+".bin";

    // 5. tile_fit_lengths
    export_tile_fit_lengths(tile_fit_lengths, "tile_fit_lengths"+std::to_string(i)+".bin");
    tile_fit_lengths.clear();
    file_string += " tile_fit_lengths"+std::to_string(i)+".bin";
  }

  // 1. multi-planar coefficients
  export_coefficients(multi_coefficients, "multi_coefficients.bin");
  multi_coefficients.clear();
  file_string += " multi_coefficients.bin";

  // 2. export multi-b_mat
  export_b_mat(*multi_mat, "multi_mat.bin");
  delete multi_mat;
  file_string += " multi_mat.bin";

  // 3. tile_fit_lengths
  export_tile_fit_lengths(multi_tile_fit_lengths, "multi_tile_fit_lengths.bin");
  multi_tile_fit_lengths.clear();
  file_string += " multi_tile_fit_lengths.bin";
  
  // 4. multi-channel plane offsets
  export_plane_offsets(plane_offsets, "plane_offsets.bin");
  plane_offsets.clear();
  file_string += " plane_offsets.bin";

  // 5. make a tar ball
  cmd += file_string;
  if (system(cmd.c_str()) == -1) {
    std::cout << "[ERROR]: 'tar' command compression failed." << std::endl;
    exit(-1);
  }

  std::string rm_files = "rm " + file_string;
  if (system(rm_files.c_str()) == -1) {
    std::cout << "[ERROR]: 'rm' command remove file failed." << std::endl;
    exit(-1);
  }
  return 0;
}

