#!/bin/bash

# Check if exactly one argument is passed to the script
if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <number>"
    exit 1
fi

# Assign the first argument to a variable
NUMBER=$1

# Print the number (optional, for verification)
echo "Number of workers to generate: $NUMBER"

# Call the python script with the number as an argument
python3 genworkers.py $NUMBER
sh hostsProg.sh
mv workers $HADOOP_HOME/etc/hadoop/slaves
# Format the HDFS namenode
hdfs namenode -format

# Start HDFS and YARN
start-dfs.sh
start-yarn.sh

# Create necessary HDFS directories
hdfs dfs -mkdir -p /user/hive/warehouse
hdfs dfs -mkdir -p /user/tmp
hdfs dfs -chmod g+w /user/tmp
hdfs dfs -chmod g+w /user/hive/warehouse
hdfs dfs -mkdir -p /user/root/

# Initialize the Hive schema
$HIVE_HOME/bin/schematool -initSchema -dbType derby
