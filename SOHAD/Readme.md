# Spark On Hadoop And Docker
## Software versions
Hadoop 2.6
https://archive.apache.org/dist/hadoop/core/hadoop-2.6.0/hadoop-2.6.0.tar.gz

Spark 1.3
https://archive.apache.org/dist/spark/spark-1.3.1/spark-1.3.1-bin-hadoop2.6.tgz

Hive 1.1.0
https://archive.apache.org/dist/hive/hive-1.1.0/apache-hive-1.1.0-bin.tar.gz

## Deploy steps

sudo rm -r /home/ubuntu/RDF_BENCH
git clone git@github.com:amesmoudi/RDF_BENCH.git
cd /home/ubuntu/RDF_BENCH
parallel-ssh -i -h hosts.txt "sudo rm -r /home/ubuntu/RDF_BENCH"
parallel-ssh -i -h hosts.txt "git clone git@github.com:amesmoudi/RDF_BENCH.git"
sh SOHAD/build-master-image.sh&

cd ..

parallel-ssh -i -h hosts.txt "sh ./RDF_BENCH/SOHAD/build-master-image.sh&"

python3 generate_compose.py 5

where 5 is the number of workers






