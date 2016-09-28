#! /bin/bash

ROOT=/home/trucndt/workspace/PSDU_Project
TARGET_IP=${2}
TARGET_USER=${1}
ENV_SETUP_PATH=/opt/poky-edison/1.6/environment-setup-core2-32-poky-linux

# Compiling
echo
echo "Compiling source code"
echo "--------------------"

## TODO: No need to remove folder everytime
rm -rf cross-build
mkdir cross-build
cd cross-build

source ${ENV_SETUP_PATH}
cmake ${ROOT}
make

# Send to target
echo
echo "Sending to " ${TARGET_USER} "@" ${TARGET_IP}
echo "--------------------"

ssh ${TARGET_USER}@${TARGET_IP} "rm /home/${TARGET_USER}/PSDU/aggregator"
scp aggregator/aggregator ${TARGET_USER}@${TARGET_IP}:/home/${TARGET_USER}/PSDU/

#remove folder after completion
cd ..
rm -rf cross-build
