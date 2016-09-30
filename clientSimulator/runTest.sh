#! /bin/bash

BDADDR="98:4F:EE:04:3E:28"
CHANNEL=1

#mkdir build; cd build; cmake ..; make
./${1} -a ${BDADDR} -c ${CHANNEL} | tee ${1}_log.txt