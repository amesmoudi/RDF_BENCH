nbworkers=4

docker stack rm sohad
sudo rm -r /hdfs/*
parallel-ssh -h ../hosts.txt sudo rm -r /hdfs/*
rm docker-compose-deploy.yml
python3 generate_compose.py $nbworkers
sleep 5s
docker stack deploy -c docker-compose-deploy.yml sohad
sleep 5s
docker stack services sohad
ssh -t root@localhost -p 2222 ./start-newCluster.sh $nbworkers
ssh -t root@localhost -p 2222 mkdir -p /rawdata/watdiv/
scp -P 2222 data/watdiv100k.nt root@localhost:/rawdata/watdiv/