      
	  AUTHOR: MATT PARKER (matthew.parker@l-3com.com)
        DATE: 5 FEB 2015
 DESCRIPTION: Instructions on how to setup Hadoop/Wine processing environment
              to extract encore data using nxcore API in parallel using 
              Hadoop streaming.			  
	  
This doesn't quite work with Hadoop Streaming yet, but here is the start of
instructions.

Preparation Instructions
========================

1. Used Hadoop Fuse to link HDFS to Linux boxes as mount point on each data node.

2. Created nxcore user/group through puppet management system on each data 
   node. Changed configuration in /etc/puppet/manifests/nodes/xdata-default.pp

3. Run 'winecfg' on each data node as the nxcore user. This will initialize the 
   /home/nxcore/.wine directories. 
	>> pssh -h datanodes runuser -l nxcore -c 'winecfg'

4. Make nxcore directory in each data node's Windows/Wine area
	>> pssh -h datanodes mkdir -p /home/nxcore/.wine/drive_c/Projects/nxcore
	
5. Symbolic link HDFS Fuse mount to Windows/Wine directory
	>> pssh -h datanodes ln -s /mnt/hdfs/data/nxcore /home/nxcore/.wine/drive_c/Projects/nxcore/data

6. Download nxcore files from github to a puppet master directory
	>> cd /srv
	>> git clone https://github.com/parker20121/nxcore.git 
	
7. Couldn't find pscp on each machine. Edited xdata-default.pp on puppet master to include the 
   pssh package. Watied until the puppet master system updated the nodes. Found pscp as 
   pscp.pssh on each node now. Not sure if this was necessary.
   
8. Copy files to wine subdirectory on data nodes from puppet master. From /srv/nxcore on puppet master, 
	>>  pscp.pssh -h /root/xdata-cluster/datanodes  hadoop-streaming/* /home/nxcore/.wine/drive_c/Projects/nxcore

9. Ensure the new directories are owned by nxcore.
	>> pssh -h /root/xdata-cluster/datanodes chown -R nxcore:nxcore /home/nxcore/.wine/drive_c
	
Testing
=======

1. Copy a subset of files to the shared directory for testing
   >>  hadoop fs -cp /SummerCamp2015/nxcore/2014010[1-5].XA.nxc /data/nxcore/

2. Running the hadoop streaming command to process the test data

    >> hadoop -jar /usr/lib/hadoop-mapreduce/hadoop-streaming-2.5.0-cdh5.3.0.jar \
	        -D mapred.min.split.size=1000737418240 \
	        -D mapred.reduce.tasks=5 \
	        -D mapred.job.name="nxcore_extract" \
	        -numRedcueTasks 0  \
	        -input hdfs://data/nxcore  \
			-output hdfs://data/nxcore/processed \
			-mapper nxprocess.sh  \
			-verbose 
			-file /home/nxcore/.wine/drive_c/Projects/nxcore/nxprocess.sh
			



