#!/bin/bash -i
source /root/.bashrc

# Assign the first argument to a variable
NUMBER=$1

# Print the number (optional, for verification)
echo "Number of workers to generate: $NUMBER"

# Call the python script with the number as an argument
python3 genworkers.py $NUMBER
python3 hostsProg.py
cp workers $HADOOP_HOME/etc/hadoop/slaves
cp workers $SPARK_HOME/conf/slaves

# Format the HDFS namenode
hdfs namenode -format

# Start HDFS and Spark
start-dfs.sh
start-master.sh
start-slaves.sh


# Create necessary HDFS directories
hdfs dfs -mkdir -p /user/hive/warehouse
hdfs dfs -mkdir -p /user/tmp
hdfs dfs -chmod g+w /user/tmp
hdfs dfs -chmod g+w /user/hive/warehouse
hdfs dfs -mkdir -p /user/root/


