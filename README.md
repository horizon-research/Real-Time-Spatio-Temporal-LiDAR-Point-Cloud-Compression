# Real-Time Spatio-Temporal LiDAR Point Cloud Compression

Point Cloud Compression (PCC) is a collection of different point cloud compression methods along with varying applcation usages.

## what is this?

This repository is a end-to-end point cloud compression algorithms. The technique leverages the temporal and spatial characterizations in point cloud.

In reality, most of objects in the real world are constructed based on certain regular shapes. It is uncommon that we cannot express those shapes in mathematically expression. Therefore, we leverage the mathematical expression to fitting a surface to a given point cloud and encode the coefficients.


Dependancies:
* OpenCV 4
* Boost Library

We only test our code in **Ubuntu 18.04**.

## How to compile

To compile the entire repository, go to the `src` directory, use `Makefile` to compile the binary.
```
 $ cd src; make
```

You will see three different bianry:
* `pcc_encoder`: an encoder to encode one point cloud frame.
* `pcc_decoder`: an decoder to decode one point cloud frame.
* `pc_test`: this is an end-to-end test suite.


## How to use

We provide a simple data sample in our `data` directory. To encode this point cloud:
```
 $ ./pcc_encoder --file ../data/0000000000.bin -p 0.18 -y 0.45 -f binary -l 4 -t 0.5 --out frame.tar.gz
```
This will produce a tar ball file named `frame.tar.gz` that contained the compressed point cloud. `--file` flag is the input point cloud in binary form. `--out` is the output compressed file.

To decompress this point cloud, use:
```
 $ ./pcc_decoder --out 0000000000.bin -p 0.18 -y 0.45 -f binary -l 4 --file frame.tar.gz
```
This will recontruct the compressed point cloud and store the recontructed point cloud into file named `0000000000.bin`.

To test the end-to-end compression, use:
```
 $  ./pcc_test --file ../data/0000000000.bin -p 0.18 -y 0.45 -f binary -l 4 -t 0.5 
```
This will give many detailed information about compression time, accuracy,etc.

## Related 
You can also check out:
  - [PCL library](http://docs.pointclouds.org/trunk/index.html): a open-sourced point cloud library.
  - [Alglib](https://www.alglib.net/): a *semi*-open-sourced algibra library.
  - [FFMPEG](https://www.ffmpeg.org/): a free video/audio compression platform.
  - [3D machine learning](https://github.com/timzhang642/3D-Machine-Learning): a collection of all recent progress in 3D machine learning.







