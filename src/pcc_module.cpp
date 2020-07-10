#include "pcc_module.h"

double map_projection(cv::Mat& f_mat, std::vector<point_cloud>& pcloud_data,
                      float pitch_precision, float yaw_precision, char option) {

#ifndef PERFORMANCE
  std::cout << "Range Map: " << f_mat.rows << ", " << f_mat.cols << std::endl;
#endif

  auto proj_start = std::chrono::high_resolution_clock::now();

  std::ios_base::sync_with_stdio(false);

  // convert pcloud into range map with x-y precision maps;
  if (option == 'e')
    pcloud_to_mat<cv::Vec4f>(pcloud_data, f_mat, pitch_precision, yaw_precision);
  else
    pcloud_to_mat<float>(pcloud_data, f_mat, pitch_precision, yaw_precision);

  auto proj_end = std::chrono::high_resolution_clock::now();

  double proj_time = std::chrono::duration_cast<std::chrono::nanoseconds>(proj_end-proj_start).count();
  proj_time *= 1e-9;

  std::cout << "Time taken by projection: " << proj_time << " sec." << std::endl;

  return proj_time;
}

void print_pcc_res(PccResult& pcc_res) {
  float avg_compression_rate, avg_loss_rate;
  avg_compression_rate = average(pcc_res.compression_rate);
  std::cout << "Compression rate: "
            << avg_compression_rate  << " bpp" << std::endl;
  float avg_restored_compression_rate = average(pcc_res.restored_compression_rate);
  std::cout << "Restored compression rate: "
            << avg_restored_compression_rate << " bpp" << std::endl;
  float avg_stream_compression_rate = average(pcc_res.stream_compression_rates);
  std::cout << "Stream compression rate: "
            << avg_stream_compression_rate << " bpp" << std::endl;
  float avg_proj_time = average(pcc_res.proj_times);
  std::cout << "2D projection time: " << avg_proj_time << " sec" << std::endl;
  float avg_fit_time = average(pcc_res.fit_times);
  std::cout << "Average fitting time: " << avg_fit_time << " sec" << std::endl;
  float avg_merge_time = average(pcc_res.merge_times);
  std::cout << "Average merging time: " << avg_merge_time << " sec" << std::endl;
  avg_loss_rate = average(pcc_res.loss_rate);
  std::cout << "Loss rate [PSNR]: " << avg_loss_rate << std::endl;
  avg_loss_rate = average(pcc_res.restored_loss_rate);
  std::cout << "Restored Loss rate [PSNR]: " << avg_loss_rate << std::endl;
  float avg_match_pct = average(pcc_res.match_pcts);
  float avg_match = average(pcc_res.matchs);
  std::cout << "Merge-Matched Percentage: " << avg_match_pct
            << " in " << avg_match << std::endl;
  float avg_unmatch_pct = average(pcc_res.unmatch_pcts);
  float avg_unmatch = average(pcc_res.unmatchs);
  std::cout << "Merge-Unmatched Percentage: " << avg_unmatch_pct
            << " in " << avg_unmatch << std::endl;
  float avg_fitting_ratio = average(pcc_res.fitting_ratios);
  std::cout << "Fitting Ratio: " << avg_fitting_ratio << std::endl;
}
