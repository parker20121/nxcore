# nxcore

This project contains code to read nxcore data files, and additional programs to split and 
load data into Hive. The applications are arranged into the following directories.

| Directory        | Description                                                                             |
|------------------|-----------------------------------------------------------------------------------------|
| nxcore_msc       | A Visual C++ program to dump data from Encore data files using the NXCORE API.          |
| hadoop-streaming | Final compiled version of nxcore-msc program, which is run on Linux using Wine.         |
| nxcore-split     | A MapReduce program to split the files by record type and date.                         |

