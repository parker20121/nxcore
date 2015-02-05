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

FILEPATH=$1
	
	# Data directory on HDFS
HDFS_DATA_DIR=hdfs://data/nxcore

	# Linux HDFS Fuse directory mount
HDFS_FUSE_DIR=/mnt/data

	# Wine location holding NXCORE binaries
WINDOWS_NXCORE_DIR=C:\\Projects\\nxcore
	
	# Wine location storing data. Need to symbolic link Linux HDFS directory to here.
WINDOWS_DATA_DIR=C:\\Projects\\nxcore\\data

	#Derive directory and file metadata. Filenames have the following form: 20140101.XA.nxc
FILENAME="${FILEPATH##*/}"
YEAR="${FILENAME:0:4}"
MONTH="${FILENAME:4:2}"
DAY="${FILENAME:6:2}"


echo "Processing $1..." > ./nxprocess.log

if [ ! -d "~/.wine/dosdevices/$WINDOWS_DATA_DIR" ]; then
	echo ln -s ${HDFS_FUSE_DIR} ${WINDOWS_DATA_DIR}
fi

echo wine ${WINDOWS_NXCORE_DIR}\\nxcore.exe ${WINDOWS_DATA_DIR}\\${FILENAME}

echo hadoop jar $HADOOP_HOME/contrib/streaming/hadoop-streaming-0.20.2-cdh3u2.jar \
	  -Dmapred.output.compress=true \
	  -Dmapred.compress.map.output=true \
	  -Dmapred.output.compression.codec=org.apache.hadoop.io.compress.BZip2Codec \
	  -Dmapred.reduce.tasks=0 \
	  -input ${HDFS_DATA_DIR}/processed/trades/${FILENAME}_TRADES.CSV \
	  -output ${HDFS_UPLOAD_DIR}/trades/${YEAR}/${MONTH}/${FILENAME}_TRADES.csv.tar.bz2 \
	  -mapper "cut -f 2"

echo hadoop jar $HADOOP_HOME/contrib/streaming/hadoop-streaming-0.20.2-cdh3u2.jar \
	  -Dmapred.output.compress=true \
	  -Dmapred.compress.map.output=true \
	  -Dmapred.output.compression.codec=org.apache.hadoop.io.compress.BZip2Codec \
	  -Dmapred.reduce.tasks=0 \
	  -input ${HDFS_DATA_DIR}/processed/exgquote/${FILENAME}_EXGQUOTE.CSV \
	  -output ${HDFS_UPLOAD_DIR}/exgquotes/${YEAR}/${MONTH}/${FILENAME}_EXGQUOTE.csv.tar.bz2 \
	  -mapper "cut -f 2"

echo hadoop jar $HADOOP_HOME/contrib/streaming/hadoop-streaming-0.20.2-cdh3u2.jar \
	  -Dmapred.output.compress=true \
	  -Dmapred.compress.map.output=true \
	  -Dmapred.output.compression.codec=org.apache.hadoop.io.compress.BZip2Codec \
	  -Dmapred.reduce.tasks=0 \
	  -input ${HDFS_DATA_DIR}/processed/mmquote/${FILENAME}_MMQUOTE.CSV \
	  -output ${HDFS_UPLOAD_DIR}/mmquotes/${YEAR}/${MONTH}/${FILENAME}_MMQUOTE.csv.tar.bz2 \
	  -mapper "cut -f 2"

echo hadoop jar $HADOOP_HOME/contrib/streaming/hadoop-streaming-0.20.2-cdh3u2.jar \
	  -Dmapred.output.compress=true \
	  -Dmapred.compress.map.output=true \
	  -Dmapred.output.compression.codec=org.apache.hadoop.io.compress.BZip2Codec \
	  -Dmapred.reduce.tasks=0 \
	  -input ${HDFS_DATA_DIR}/processed/symbolchange/${FILENAME}_SYMBOLCHANGE.CSV \
	  -output ${HDFS_UPLOAD_DIR}/symbolchages/${YEAR}/${MONTH}/${FILENAME}_SYMBOLCHANGE.csv.tar.bz2 \
	  -mapper "cut -f 2"

echo hadoop fs -rm -skipTrash ${HDFS_DATA_DIR}/processed/trades/${FILENAME}_TRADES.CSV
echo hadoop fs -rm -skipTrash ${HDFS_DATA_DIR}/processed/exgquote/${FILENAME}_EXGQUOTE.CSV
echo hadoop fs -rm -skipTrash ${HDFS_DATA_DIR}/processed/mmquote/${FILENAME}_MMQUOTE.CSV
echo hadoop fs -rm -skipTrash ${HDFS_DATA_DIR}/processed/symbolchange/${FILENAME}_SYMBOLCHANGE.CSV
