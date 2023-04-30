# Real-Time Spatio-Temporal LiDAR Point Cloud Compression

Point Cloud Compression (PCC) is a collection of different point cloud compression methods along with varying applcation usages.

## What is this?

This repository is an end-to-end point cloud compression algorithms. The technique leverages the temporal and spatial characterizations in point cloud.

In reality, most of objects in the real world are constructed based on certain regular shapes. It is uncommon that we cannot express those shapes in mathematically expression. Therefore, we leverage the mathematical expression to fitting a surface to a given point cloud and encode the coefficients.

For more details, please check out our paper: [Real-Time Spatio-Temporal LiDAR Point Cloud Compression](https://arxiv.org/pdf/2008.06972.pdf)

Dependancies:
* OpenCV4
* Boost Library

We only test our code in **Ubuntu 18.04**.

## How to compile

To compile the entire repository, go to the `src` directory, use `Makefile` to compile the binary.
```
 $ cd src; make
```

**Note**: in `Makefile`, we use `OpenCV4`, but you can change it to different version if necessary.

You will see three different bianry:
* `pcc_encoder`: an encoder to encode one point cloud frame.
* `pcc_decoder`: an decoder to decode one point cloud frame.
* `pcc_test`: this is an end-to-end test suite.
* `pcc_stream_encoder`: an encoder to encode a stream of multiple point cloud frames.
* `pcc_stream_decoder`: an decoder to decode a stream of multiple point cloud frames.
* `pcc_stream_test` : this is an end-to-end test for streaming compression.


## How to use

### Single frame compression and decompression

We provide a simple data sample in our `data` directory. To encode this point cloud:
```
 $ ./pcc_encoder --path ../data --file 0000000000.bin -p 0.18 -y 0.45 -f binary -l 4 -t 0.5 --out frame.tar.gz
```
This will produce a tar ball file named `frame.tar.gz` that contained the compressed point cloud. `--file` flag is the input point cloud in binary form. `--out` is the output compressed file.

To decompress this point cloud, use:
```
 $ ./pcc_decoder -p 0.18 -y 0.45 -f binary -l 4 --file frame.tar.gz
```
This will recontruct the compressed point cloud and store the recontructed point cloud into file named `0000000000.bin`.

To test the end-to-end compression, use:
```
 $  ./pcc_test --file ../data/0000000000.bin -p 0.18 -y 0.45 -f binary -l 4 -t 0.5 
```
This will give many detailed information about compression time, accuracy,etc.

### Multi-frame compression and decompression

To compress multiple frame together, we can use:
```
 $ ./pcc_stream_encoder  -p 0.18 -y 0.45 -f binary -l 4 -t 0.5 --out frames.tar.gz --input-path ../data --input-files  0000000000.bin  0000000001.bin  0000000002.bin  0000000003.bin  0000000004.bin
```
This command will compress 5 point clouds (0000000000.bin-0000000004.bin) from directory ` ../data`. The output result stores in `frames.tar.gz`.

To recover these 5 point clouds, use the command:
```
 $ ./pcc_stream_decoder  -p 0.18 -y 0.45 -f binary -l 4 --input frames.tar.gz
```
This will reverse the compression process and generate 5 raw point cloud data. `frames.tar.gz` can automatically store the ortiginal filenames.

To test the end-to-end compression and decompression:
```
 $ ./pcc_stream_test  -p 0.18 -y 0.45 -f binary -l 4 -t 0.5 --out frames.tar.gz --input-path ../data --input-files  0000000000.bin  0000000001.bin  0000000002.bin  0000000003.bin  0000000004.bin
```

### Common flag meanings

- p: horizontal degree granularity. `0.18` stands for 0.18 degree.
- y: vertical degree granularity. `0.45` stands for 0.45 degree.
- f: is the data format, typically the data format is binary.
- l: tile dimension, `4` stands for 4x4 tile dimension.
- t: error threshold. `0.5` stands for when fitting the point clouds, the error between fitted value and original one is below 0.5.

### Note

These compression encoder and decoder are designed for [KITTI dataset](http://www.cvlibs.net/datasets/kitti/) and LiDAR device, [Velodyne HDL-64E](https://velodynelidar.com/products/hdl-64e/). More specifically, the vertical angle range for point cloud is from -25 degree to 5 degree. For other application settings, please check out [`config.h`](https://github.com/horizon-research/Real-Time-Spatio-Temporal-LiDAR-Point-Cloud-Compression/blob/master/src/config.h) and change parameters, `ROW_OFFSET`, `COL_OFFSET`, `VERTICAL_DEGREE` and `HORIZONTAL_DEGREE`.

The compression rate from this implementation will be lower than the number from the original paper, because we use more compact bit representation for encoded data. In this repo implementation, the data representation is looser, this implementation use the common c++ data types like CHAR, INT, FLOAT, SHORT, etc. Therefore, the compression rate will decrease to some extents.

## Citation

If you think this work is useful in your research, please consider cite our paper:
```
@article{yu2020pcc,
  title={Real-Time Spatio-Temporal LiDAR Point Cloud Compression},
  author={Feng, Yu and Liu, Shaoshan and Zhu, Yuhao},
  journal={2020 IEEE/RSJ International Conference on Intelligent Robots and Systems(IROS)},
  year={2020}
}
```

## Related

For more experimental data, please check out raw data from [KITTI](http://www.cvlibs.net/datasets/kitti/). You can directly use the Velodyne data directly, just need to specify the data path and input data names during encoding and decoding.

You can also check out:
  - [PCL library](http://docs.pointclouds.org/trunk/index.html): a open-sourced point cloud library.
  - [Alglib](https://www.alglib.net/): a *semi*-open-sourced algibra library.
  - [FFMPEG](https://www.ffmpeg.org/): a free video/audio compression platform.
  - [3D machine learning](https://github.com/timzhang642/3D-Machine-Learning): a collection of all recent progress in 3D machine learning.







