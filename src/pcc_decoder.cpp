/*
 * This is an example of 3d lidar compression.
 * */

#include "pcc_module.h"
#include "encoder.h"
#include "decoder.h"
#include "io.h"

int main(int argc, char** argv) { 
  
  std::string file_path, out_file;
  std::string input_format("binary");
  float pitch_precision, yaw_precision, threshold;
  int tile_size;

  namespace po = boost::program_options;

  po::options_description opts("PCC options");
  opts.add_options()
    ("help,h", "Print help messages")
    ("file", po::value<std::string>(&file_path)->required(), "compressed data filename")
    ("pitch,p", po::value<float>(&pitch_precision)->required(), "pitch precision")
    ("yaw,y", po::value<float>(&yaw_precision)->required(), "yaw precision")
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
  
  /*******************************************************************/
  // initialization

  int row = (VERTICAL_DEGREE/yaw_precision);
  row = ((row + tile_size-1)/tile_size)*tile_size;
  int col = HORIZONTAL_DEGREE/pitch_precision + tile_size;
  col = ((col + tile_size-1)/tile_size)*tile_size;

  int mat_div_tile_sizes[] = {row/tile_size, col/tile_size};

  // untar the compressed point cloud
  std::string cmd = "tar -xzvf " + file_path;
  if (system(cmd.c_str()) == -1) {
    std::cout << "[ERROR]: 'tar' command executed failed." << std::endl;
     exit(-1);
  }

  std::string file_string;
  std::vector<std::string> output_list;
  import_filenames(output_list, "filenames.bin");
  file_string += " filenames.bin";
  out_file = output_list[0];

  // what we need to store:
  // 1. b_mat: binary map for tile type
  cv::Mat* b_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
  import_b_mat(*b_mat, "b_mat.bin");
  file_string += " b_mat.bin";

  // 2. planar coefficients
  std::vector<cv::Vec4f> coefficients;
  import_coefficients(coefficients, "coefficients.bin");
  file_string += " coefficients.bin";

  // 3. occ_mat: occupation map
  cv::Mat* occ_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
  import_occ_mat(*occ_mat, "occ_mat.bin");
  file_string += " occ_mat.bin"; 
  
  // 4. unfit_nums: unfitted_nums
  std::vector<float> unfit_nums;
  import_unfit_nums(unfit_nums, "unfit_nums.bin");
  file_string += " unfit_nums.bin";
  
  // 5. tile_fit_lengths
  std::vector<int> tile_fit_lengths;
  import_tile_fit_lengths(tile_fit_lengths, "tile_fit_lengths.bin");
  file_string += " tile_fit_lengths.bin";

  // reconstruct the range image
  cv::Mat* r_mat = new cv::Mat(row, col, CV_32FC1, 0.f);
  // decoding
  decoder::single_channel_decode(*r_mat, *b_mat, mat_div_tile_sizes, coefficients, 
                                 *occ_mat, tile_fit_lengths, unfit_nums, tile_size);

  std::vector<point_cloud> restored_pcloud;
  restore_pcloud(*r_mat, pitch_precision, yaw_precision, restored_pcloud);
  
  pcloud2bin(out_file, restored_pcloud);
  // output_cloud(pcloud_data, "org.ply");
  // output_cloud(restored_pcloud, "restored.ply");
  std::cout << "**********************************************************" << std::endl;
  
  delete r_mat;
  delete b_mat;
  delete occ_mat;
  
  cmd = "rm " + file_string;
  if (system(cmd.c_str()) == -1) {
    std::cout << "[ERROR]: 'rm' command executed failed." << std::endl;
     exit(-1);
  }

  return 0;
}

