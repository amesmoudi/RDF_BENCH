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
mv workers $HADOOP_HOME/etc/hadoop/workers


# Start HDFS and YARN
start-dfs.sh
start-yarn.sh