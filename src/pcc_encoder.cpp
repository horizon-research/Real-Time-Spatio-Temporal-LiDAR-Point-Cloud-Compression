/*
 * This is an example of converting 3d lidar into 3d img for compression.
 * */

#include "pcc_module.h"
#include "encoder.h"

int main(int argc, char** argv) { 
  
  std::string file_path;
  std::string input_format("binary");
  float pitch_precision, yaw_precision, threshold;
  int tile_size;

  namespace po = boost::program_options;

  po::options_description opts("PCC options");
  opts.add_options()
    ("help,h", "Print help messages")
    ("file", po::value<std::string>(&file_path)->required(), "raw point cloud data path")
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
  load_pcloud(file_path, pcloud_data);

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
  
  std::cout << "Loss rate [PSNR]: " << psnr << " Compression rate: "
            << pcc_res.compression_rate->back() << " bpp." << std::endl;
  
  /*******************************************************************/
  // fitting range map
  std::vector<cv::Vec4f> coefficients;
  std::vector<int> tile_fit_lengths;
  std::vector<float> unfit_nums;
  std::vector<int> code;

  cv::Mat* b_mat = new cv::Mat(row/tile_size, col/tile_size, CV_32SC1, 0.f);
  
  encoder::single_channel_fit(*f_mat, *b_mat, mat_div_tile_sizes, coefficients, 
                              tile_fit_lengths, unfit_nums, unfit_code,
                              threshold, tile_size);
  
  // pcc_res.fit_times->push_back(fit_time);
  
  /*
  // loss error compute;
  for (int j = 0; j < channel; j++) {
    auto& t_pcloud = t_frames[j];
    psnr = compute_loss_rate<cv::Vec4f>(*(r_mat[j]), t_pcloud, pitch_precision, yaw_precision);
    pcc_res.restored_loss_rate->push_back(psnr);
    std::cout << "[restore] psnr: " << psnr << std::endl;
    
    std::vector<point_cloud> r_pcloud;
    restore_pcloud<cv::Vec4f>(*(r_mat[j]), t_pcloud, pitch_precision, yaw_precision, 1, r_pcloud);
    auto move = moves[i+j];
    // r_pcloud = translate_pcloud(r_pcloud, move, -channel_times[j]);
    // output pcloud
    char ply_file[64];
    sprintf(ply_file, "./ply/%06d.ply", (i+j));
    // output_cloud(r_pcloud, ply_file);
    
    rec_pcloud_cnt += r_pcloud.size();
  
    if (out_path.size() > 0) {
      // output pcloud
      char out_file[64];
      sprintf(out_file, "./%s/%06d.bin", out_path.c_str(), (i+j));
      pcloud2bin(std::string(out_file), r_pcloud);
    }
  } */
  
  std::cout << "**********************************************************" << std::endl;
  
  // print_pcc_res(pcc_res);

  // std::cout << "[Original]: Pcloud size: " << org_pcloud_cnt / frame_cnt << std::endl;
  // std::cout << "[Restore]: Pcloud size: " << rec_pcloud_cnt / frame_cnt << std::endl;

  return 0;
}

