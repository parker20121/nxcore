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
#

cd /home/nxcore/.wine/drive_c/Projects/nxcore

FILELIST="/mnt/hdfs/data/nxcore/filelist/$(hostname).txt"

        # Wine location holding NXCORE binaries
WINDOWS_NXCORE_DIR=c:\\Projects\\nxcore

        # Wine location storing data. Need to symbolic link Linux HDFS directory to here.
WINDOWS_DATA_DIR=c:\\Projects\\nxcore\\data


cat $FILELIST | while read FILENAME; do

   echo "Processing ${FILENAME}..."
   echo "NXCORE: ${WINDOWS_NXCORE_DIR}\\nxcore.exe"
   echo "  FILE: ${WINDOWS_DATA_DIR}\\${FILENAME}"
   echo "  TEMP: /mnt/hdfs/data/nxcore/processed/${FILENAME}.txt.tmp"
   echo "OUTPUT: /mnt/hdfs/data/nxcore/processed/${FILENAME}.txt"

   wine ${WINDOWS_NXCORE_DIR}\\nxcore.exe ${WINDOWS_DATA_DIR}\\${FILENAME} > /mnt/hdfs/data/nxcore/processed/$FILENAME.txt.tmp

   mv /mnt/hdfs/data/nxcore/processed/${FILENAME}.txt.tmp /mnt/hdfs/data/nxcore/processed/${FILENAME}.txt

   echo "Done processing ${FILENAME}."

done

