#!/usr/bin/env bash

#set the KOS environtment variables
source /opt/toolchains/dc/kos/environ.sh
DCTRACE=1 ENJ_DEBUG=1 make clean
ENJ_SHOWFRAMETIMES=1 ENJ_SUPERSAMPLING=1 ENJ_DEBUG=1 OPTLEVEL=g make -j 44
find . -name '*.o' -exec rm {} \; 
exit