#!/bin/bash

# source /usr/lib/hadoop/etc/hadoop/hadoop-env.shi
# source /etc/hadoop/conf/hadoop-env.sh

hadoop fs -rm -r hdfs://xdata/data/nxcore/job

# export DEFAULT_MAPREDUCE_APPLICATION_CLASSPATH=$HADOOP_MAPRED_HOME/share/hadoop/mapreduce/*,$HADOOP_MAPRED_HOME/share/hadoop/mapreduce/lib/*

# env

hadoop jar /usr/lib/hadoop-mapreduce/hadoop-streaming.jar \
                -D mapreduce.input.fileinputformat.split.minsize=1000737418240 \
                -D mapreduce.job.reduces=0  \
                -input hdfs://xdata/data/nxcore/filelist  \
                -output hdfs://xdata/data/nxcore/job \
                -mapper nxprocess.sh \
                -file /home/nxcore/.wine/drive_c/Projects/nxcore/nxprocess.sh \
                -verbose
