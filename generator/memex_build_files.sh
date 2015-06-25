DATANODES=/root/memex-cluster/datanodes
NXCORE_HDFS_DATA_DIR=/parker/nxcore/raw/dataset2

mkdir -p filelist/2014/07
mkdir -p filelist/2014/08
mkdir -p filelist/2014/09
mkdir -p filelist/2014/10
mkdir -p filelist/2014/11
mkdir -p filelist/2014/12

rm -f filelist/2014/07/*.txt
rm -f filelist/2014/08/*.txt
rm -f filelist/2014/09/*.txt
rm -f filelist/2014/10/*.txt
rm -f filelist/2014/11/*.txt
rm -f filelist/2014/12/*.txt

hdfs dfs -ls $NXCORE_HDFS_DATA_DIR/201407* > filelist_201407.txt
hdfs dfs -ls $NXCORE_HDFS_DATA_DIR/201408* > filelist_201408.txt
hdfs dfs -ls $NXCORE_HDFS_DATA_DIR/201409* > filelist_201409.txt
hdfs dfs -ls $NXCORE_HDFS_DATA_DIR/201410* > filelist_201410.txt
hdfs dfs -ls $NXCORE_HDFS_DATA_DIR/201411* > filelist_201411.txt
hdfs dfs -ls $NXCORE_HDFS_DATA_DIR/201412* > filelist_201412.txt

ruby filelist.rb $DATANODES filelist_201407.txt filelist/2014/07
ruby filelist.rb $DATANODES filelist_201408.txt filelist/2014/08
ruby filelist.rb $DATANODES filelist_201409.txt filelist/2014/09
ruby filelist.rb $DATANODES filelist_201410.txt filelist/2014/10
ruby filelist.rb $DATANODES filelist_201411.txt filelist/2014/11
ruby filelist.rb $DATANODES filelist_201412.txt filelist/2014/12

hdfs dfs -mkdir -p /data/nxcore/filelist/2014/07
hdfs dfs -mkdir -p /data/nxcore/filelist/2014/08
hdfs dfs -mkdir -p /data/nxcore/filelist/2014/09
hdfs dfs -mkdir -p /data/nxcore/filelist/2014/10
hdfs dfs -mkdir -p /data/nxcore/filelist/2014/11
hdfs dfs -mkdir -p /data/nxcore/filelist/2014/12

hdfs dfs -rm /data/nxcore/filelist/2014/07/*
hdfs dfs -rm /data/nxcore/filelist/2014/08/*
hdfs dfs -rm /data/nxcore/filelist/2014/09/*
hdfs dfs -rm /data/nxcore/filelist/2014/10/*
hdfs dfs -rm /data/nxcore/filelist/2014/11/*
hdfs dfs -rm /data/nxcore/filelist/2014/12/*

hdfs dfs -copyFromLocal filelist/2014/07/*.txt /data/nxcore/filelist/2014/07/.
hdfs dfs -copyFromLocal filelist/2014/08/*.txt /data/nxcore/filelist/2014/08/.
hdfs dfs -copyFromLocal filelist/2014/09/*.txt /data/nxcore/filelist/2014/09/.
hdfs dfs -copyFromLocal filelist/2014/10/*.txt /data/nxcore/filelist/2014/10/.
hdfs dfs -copyFromLocal filelist/2014/11/*.txt /data/nxcore/filelist/2014/11/.
hdfs dfs -copyFromLocal filelist/2014/12/*.txt /data/nxcore/filelist/2014/12/.
