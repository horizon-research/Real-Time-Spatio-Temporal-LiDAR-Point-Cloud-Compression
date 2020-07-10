#!/bin/bash

MAIN_DIR="../../pc_compression/data/"
DATA_PATH="kitti_slam/dataset/sequences/05/velodyne/"

PRECISION=("18")
# THRESHOLD=("2")

echo "## START TO PROFILE THRESHOLD = 0.${i} ##"
for j in "${PRECISION[@]}"
do
  echo "precision ${j}"
  for n in {000000..000010}
  do
    ./pcc_encoder --file ${MAIN_DIR}/${DATA_PATH}/${n}.bin \
      -p 0.${j} -y 0.45 -f binary -l 4 -t 0.5
    echo "[done] ${n}"
  done
done
