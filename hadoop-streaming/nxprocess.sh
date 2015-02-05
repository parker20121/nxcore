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
#  usage: hadoop 
#

   # If no argument, stop processing.
#if [ "$1" -eq "" ] 
#then
#	exit 0
#fi

FILEPATH=$1

	#Set constants
HDFS_UPLOAD_DIR=/nxcore/trade
WINDOWS_APPLICATION_DIR=C:/Projects

	#Derive metadata. Filenames in the form of 20140101.XA.nxc
FILENAME="${FILEPATH##*/}"
YEAR="${FILENAME:0:4}"
MONTH="${FILENAME:4:2}"
DAY="${FILENAME:6:2}"


echo "Filepath: $FILEPATH"
echo "Filename: $FILENAME"
echo "Year: $YEAR"
echo "Month: $MONTH"
echo "Day: $DAY"

echo "Processing $1..." > nxprocess.log

echo "hadoop fs -copyToLocal $FILEPATH $FILENAME"
echo wine $WINDOWS_APPLICATION_DIR/nxcore.exe $WINDOWS_APPLICATION_DIR/$FILENAME

echo tar --bzip2 -C /processed/trade -cvf ${FILENAME}_TRADES.csv.tar ${FILENAME}_TRADES.CSV
echo hadoop fs -moveFromLocal $FILENAME_TRADES.csv.tar.bz2 $HDFS_UPLOAD_DIR/$YEAR/$MONTH/$FILENAME_TRADES.csv.tar.bz2

echo tar --bzip2 -C /processed/exgquote -cvf $FILENAME_EXGQUOTE.csv.tar $FILENAME_EXGQUOTE.CSV
echo hadoop fs -moveFromLocal $FILENAME_EXGQUOTE.csv.tar.bz2 $HDFS_UPLOAD_DIR/$YEAR/$MONTH/$FILENAME_EXGQUOTE.csv.tar.bz2

echo tar --bzip2 -C /processed/mmquote -cvf $FILENAME_MMQUOTE.csv.tar $FILENAME_MMQUOTE.CSV
echo hadoop fs -moveFromLocal $FILENAME_MMQUOTE.csv.tar.bz2 $HDFS_UPLOAD_DIR/$YEAR/$MONTH/$FILENAME_MMQUOTE.csv.tar.bz2

echo tar --bzip2 -C /processed/symbolchange -cvf $FILENAME_SYMBOLCHANGE.csv.tar $FILENAME_SYMBOLCHANGE.CSV
echo hadoop fs -moveFromLocal $FILENAME_SYMBOLCHANGE.csv.tar.bz2 $HDFS_UPLOAD_DIR/$YEAR/$MONTH/$FILENAME_SYMBOLCHANGE.csv.tar.bz2

echo rm -rf processed