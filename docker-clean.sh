docker container rm -f $(docker container ls -aq)
docker network prune -f
docker volume prune -f
docker rmi $(docker images -q) -f