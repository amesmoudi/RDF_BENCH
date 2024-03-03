# RDF_BENCH
## AdPart
docker compose build base_image  

docker compose build worker  

docker compose build master

docker compose up master worker

docker swarm init

parallel-ssh -i -h hosts.txt "docker node list"

parallel-ssh -i -h hosts.txt "docker swarm join --token SWMTKN-1-0b7egaglgpmicryg8jtaj1l9s473lbo3vw6b3avahumblgadwf-3o7hfqits7qurli59poawyco1 192.168.100.208:2377"

docker node list
 
cd /home/ubuntu/RDF_BENCH/AdPart/;docker compose build base_image
cd /home/ubuntu/RDF_BENCH/AdPart/;docker compose build master

parallel-ssh -i -h hosts.txt "cd /home/ubuntu/RDF_BENCH/AdPart/;docker compose build base_image"
parallel-ssh -i -h hosts.txt "cd /home/ubuntu/RDF_BENCH/AdPart/;docker compose build worker"


docker stack rm adpart

docker stack deploy -c docker-compose.yml adpart


docker stack services adpart

docker service ps adpart_master


docker build -t adpart-master:1.0 Adpa

docker run -p 2222:22 adpart-master:latest 
