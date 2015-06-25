#!/bin/bash
#
#      AUTHOR: Matt Parker (matthew.parker@l-3com.com)
#     COMPANY: L3 Data Tactics
#        DATE: February 2, 2015
# DESCRIPTION: Program to extract nxcore data using the Hadoop Streaming process.
#              This is a script called from Hadoop by running a Windows C++ program
#              that's called by a Hadoop Streaming process. The final result is compressed
#              by BZip2 and stored back to HDFS.
#
#  I put it under Wine's directories. On my machine that is /home/matt/.wine/dosdevices/c:/Projects/nxcore
#  then run the program with the following command with full paths to both files:
#  wine c:\\Projects\\nxcore\\nxcore.exe c:\\Projects\\nxcore\\20140101.XA.nxc
#
#  CHANGE HISTORY
#  ==============
#
#  MAP - 06/24/2015 - Added additional variables to clean up code. Changed bzip compression level from c9 to c5.
#

   # Make sure we're in the proper processing 
   # directory no matter where script in envoked.
cd /home/nxcore/.wine/drive_c/Projects/nxcore

   # BZIP2 compression level 1-9 (least-best)
COMPRESSION_LEVEL=5

   # Parent directory where NXCORE stores temp and processed directory
NXCORE_FUSE_DIR=/mnt/hdfs/data/nxcore

   # Subdirectory to store processing results and filelists
TIMEFRAME="2014/08"

   # Where list of files to process lives on HDFS through fuse mount
FILELIST="${NXCORE_FUSE_DIR}/filelist/${TIMEFRAME}/$(hostname).txt"

   # Wine location on it's virtual C drive holding the NXCORE binaries
WINDOWS_NXCORE_DIR=c:\\Projects\\nxcore

   # Wine location storing data. Need to 
   # symbolic link Linux HDFS directory to here
WINDOWS_DATA_DIR=c:\\Projects\\nxcore\\data

   # NXCore output stored on HDFS output directory over fuse mount path
PROCESSED_DIRECTORY=${NXCORE_FUSE_DIR}/processed/${TIMEFRAME}

   # Cycle through files for processing
cat $FILELIST | while read FILENAME; do

   echo "Processing ${FILENAME}..."
   echo "NXCORE: ${WINDOWS_NXCORE_DIR}\\nxcore.exe"
   echo "  FILE: ${WINDOWS_DATA_DIR}\\${FILENAME}"
   echo "  TEMP: ${PROCESSED_DIRECTORY}/${FILENAME}.txt.tmp"
   echo "OUTPUT: ${PROCESSED_DIRECTORY}/${FILENAME}.txt"

   wine ${WINDOWS_NXCORE_DIR}\\nxcore.exe ${WINDOWS_DATA_DIR}\\${FILENAME} | bzip2 -c$COMPRESSION_LEVEL > $PROCESSED_DIRECTORY/$FILENAME.txt.bz2.tmp

   mv $PROCESSED_DIRECTORY/${FILENAME}.txt.bz2.tmp $PROCESSED_DIRECTORY/${FILENAME}.bz2

   echo "Done processing ${FILENAME}."

done

