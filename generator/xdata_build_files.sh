rm -f filelist/2014/07/*.txt
rm -f filelist/2014/08/*.txt
rm -f filelist/2014/09/*.txt
rm -f filelist/2014/10/*.txt
rm -f filelist/2014/11/*.txt
rm -f filelist/2014/12/*.txt

hdfs dfs -ls /SummerCamp2015/nxcore/raw/dataset2/201407* > filelist_201407.txt
hdfs dfs -ls /SummerCamp2015/nxcore/raw/dataset2/201408* > filelist_201408.txt
hdfs dfs -ls /SummerCamp2015/nxcore/raw/dataset2/201409* > filelist_201409.txt
hdfs dfs -ls /SummerCamp2015/nxcore/raw/dataset2/201410* > filelist_201410.txt
hdfs dfs -ls /SummerCamp2015/nxcore/raw/dataset2/201411* > filelist_201411.txt
hdfs dfs -ls /SummerCamp2015/nxcore/raw/dataset2/201412* > filelist_201412.txt

ruby xdata_filelist.rb /root/xdata-cluster/datanodes filelist_201407.txt filelist/2014/07
ruby xdata_filelist.rb /root/xdata-cluster/datanodes filelist_201408.txt filelist/2014/08
ruby xdata_filelist.rb /root/xdata-cluster/datanodes filelist_201409.txt filelist/2014/09
ruby xdata_filelist.rb /root/xdata-cluster/datanodes filelist_201410.txt filelist/2014/10
ruby xdata_filelist.rb /root/xdata-cluster/datanodes filelist_201411.txt filelist/2014/11
ruby xdata_filelist.rb /root/xdata-cluster/datanodes filelist_201412.txt filelist/2014/12

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
