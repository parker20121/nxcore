hadoop fs -rm -R /data/nxcore/split
hadoop -jar split-1.0-SNAPSHOT mil.darpa.nxcore.SplitFiles /data/nxcore/processed /data/nxcore/split
