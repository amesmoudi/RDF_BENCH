#!/bin/bash

# Initialize variables
queriesPath=""
dbName=""
sUB=""

# Process command line arguments
while getopts ":q:d:s:" opt; do
  case $opt in
    q)
      queriesPath=$OPTARG  # Set queriesPath with the argument value for -q
      ;;
    d)
      dbName=$OPTARG  # Set dbName with the argument value for -d
      ;;
    s)
      sUB=$OPTARG  # Set sUB with the argument value for -s
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

# Define paths using the variables
destPath=/data/tmp/data/$dbName/queries/
dataSetPath=/data/tmp/data/$dbName/
dbPath=/user/root/data/$dbName/

# Remove the destination directory if it exists
rm -rf $destPath

# Create the destination directory
mkdir -p $destPath

# Generate SQL queries
python3 /opt/workspace/QueryTranslator/WatDivQuerySet/translateWatDivQueries.py -s $queriesPath -t $destPath -d $dataSetPath -u $sUB

# Execute queries
python2 /opt/workspace/QueryExecutor/QueryExecutor.py -d $dbPath -q $destPath/compositeQueryFile.txt
