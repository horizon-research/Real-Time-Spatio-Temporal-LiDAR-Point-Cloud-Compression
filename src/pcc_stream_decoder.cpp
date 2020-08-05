/*
 * This is an example of 3d lidar compression.
 * */

#include <stdlib.h>
#include "pcc_module.h"
#include "encoder.h"
#include "decoder.h"
#include "io.h"

int main(int argc, char** argv) { 
  
  std::string input_file;
  std::string input_format("binary");
  float pitch_precision, yaw_precision, threshold;
  int tile_size;

  namespace po = boost::program_options;

  po::options_description opts("PCC options");
  opts.add_options()
    ("help,h", "Print help messages")
    ("input", po::value<std::string>(&input_file)->required(), "compressed data filename")
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
  
  PccResult pcc_res;

  /*******************************************************************/
  // initialization

  int row = (VERTICAL_DEGREE/yaw_precision);
  row = ((row + tile_size-1)/tile_size)*tile_size;
  int col = HORIZONTAL_DEGREE/pitch_precision + tile_size;
  col = ((col + tile_size-1)/tile_size)*tile_size;

  int mat_div_tile_sizes[] = {row/tile_size, col/tile_size};

  std::string cmd = "tar -xzvf " + input_file;
  if (system(cmd.c_str())) {
      std::cout << "[ERROR]: 'tar' command decompression failed." << std::endl;
      exit(-1);
  }
  
  std::string file_string;
  // import filenames
  std::vector<std::string> output_list;
  import_filenames(output_list, "filenames.bin");
  file_string += " filenames.bin";
  
  /*******************************************************************/
  // import occupation maps
  std::vector<cv::Mat*> occ_mats;
  for (int i = 0; i < output_list.size(); i++) {
    cv::Mat* occ_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
    import_occ_mat(*occ_mat, "occ_mat"+std::to_string(i)+".bin");
    occ_mats.push_back(occ_mat);
    file_string += " occ_mat"+std::to_string(i)+".bin";
  }

  // import the coefficients for mutli-plane fitting
  // 1. multi-planar coefficients
  std::vector<cv::Vec4f> multi_coefficients;
  import_coefficients(multi_coefficients, "multi_coefficients.bin");
  file_string += " multi_coefficients.bin";

  // 2. multi-b_mat
  cv::Mat*  multi_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
  import_b_mat(*multi_mat, "multi_mat.bin");
  file_string += " multi_mat.bin";

  // 3. tile_fit_lengths
  std::vector<int> multi_tile_fit_lengths;
  import_tile_fit_lengths(multi_tile_fit_lengths, "multi_tile_fit_lengths.bin");
  file_string += " multi_tile_fit_lengths.bin";

  // 4. multi-channel plane offsets
  std::vector<std::vector<float>> plane_offsets;
  import_plane_offsets(plane_offsets, "plane_offsets.bin", output_list.size());
  file_string += " plane_offsets.bin";

  std::vector<cv::Mat*> r_mats;
  for (int i = 0; i < output_list.size(); i++) {
    cv::Mat* r_mat = new cv::Mat(row, col, CV_32FC1, 0.f);
    r_mats.push_back(r_mat);
  }

  // decode and reconstruct range image into r_mat
  decoder::multi_channel_decode(r_mats, *multi_mat, mat_div_tile_sizes,
                                occ_mats, multi_coefficients,
                                plane_offsets, multi_tile_fit_lengths, 
                                threshold, tile_size);


  for (int i = 0; i < output_list.size(); i++) {

    auto r_mat = r_mats[i];
    auto occ_mat = occ_mats[i];
    
    cv::Mat* b_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
    std::vector<cv::Vec4f> coefficients;
    std::vector<int> tile_fit_lengths;
    std::vector<float> unfit_nums;

    import_b_mat(*b_mat, "b_mat"+std::to_string(i)+".bin");
    file_string += " b_mat"+std::to_string(i)+".bin";
    
    import_coefficients(coefficients, "coefficients"+std::to_string(i)+".bin");
    file_string += " coefficients"+std::to_string(i)+".bin";

    import_unfit_nums(unfit_nums, "unfit_nums"+std::to_string(i)+".bin");
    file_string += " unfit_nums"+std::to_string(i)+".bin";

    import_tile_fit_lengths(tile_fit_lengths, "tile_fit_lengths"+std::to_string(i)+".bin");
    file_string += " tile_fit_lengths"+std::to_string(i)+".bin";

    // decoding
    decoder::single_channel_decode(*r_mat, *b_mat, mat_div_tile_sizes, 
                                   coefficients, *occ_mat, tile_fit_lengths,
                                   unfit_nums, tile_size, multi_mat);
  
    std::vector<point_cloud> restored_pcloud;
    restore_pcloud(*r_mat, pitch_precision, yaw_precision, restored_pcloud);
    
    pcloud2bin(output_list[i], restored_pcloud);

    // delete b_mat;
  }
  
  std::string rm_cmd = "rm " + file_string;
  if (system(rm_cmd.c_str())) {
      std::cout << "[ERROR]: 'rm' command deletion failed." << std::endl;
      exit(-1);
  }

  delete multi_mat;
  std::cout << "**********************************************************" << std::endl;

  return 0;
}

