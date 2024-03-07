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
sh SOHAD/build-master-image.sh > build-master.log &


parallel-ssh -i -h hosts.txt "nohup sh ./RDF_BENCH/SOHAD/build-worker-image.sh > build-worker.log 2>&1 &"
parallel-ssh -i -h hosts.txt "tail -f build-worker.log"

## Run a new cluster
rm docker-compose-deploy.yml
python3 SOHAD/generate_compose.py 7

where 7 is the number of workers

2. Deploy the `sohad` stack using the `docker-compose-deploy.yml` file:

```bash
docker stack deploy -c docker-compose-deploy.yml sohad
```

3. Check the status of the services:

```bash
docker stack services sohad
```

### from the local machine
ssh root@10.16.14.170 -p 2222 -L 50070:master:50070 -L 8088:master:8088
sh start-newCluster.sh 7





## Clean and run a new cluster
docker stack rm sohad
cd ~/RDF_BENCH/
sudo rm -r /hdfs/*
parallel-ssh -h hosts.txt sudo rm -r /hdfs/*






