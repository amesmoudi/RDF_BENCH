
# RDF_BENCH
RDF_BENCH is a distributed RDF benchmark system designed to evaluate the performance of query processing over large RDF datasets.
## AdPart

 This guide will assist you in deploying and running AdPart using Docker Swarm.

### Prerequisites

- Docker and Docker Compose installed on all machines.
- A `hosts.txt` file containing the IP addresses of all the machines in the cluster.

### Environment Initialization

1. Create a `/data` directory on all machines to store data:

```bash
parallel-ssh -i -h hosts.txt "sudo mkdir /data"
parallel-ssh -i -h hosts.txt "sudo chown ubuntu /data"
```

2. Initialize Docker Swarm on the master node:

```bash
docker swarm init
```

3. Join the other nodes to the Swarm cluster using the token generated by the Swarm initiation:

```bash
parallel-ssh -i -h hosts.txt "docker swarm join --token [SWARM_TOKEN] [MASTER_IP]:2377"
```
Replace `[SWARM_TOKEN]` with the actual token and `[MASTER_IP]` with the master node's IP address.

4. Verify that all nodes have joined the cluster:

```bash
docker node list
```

### Building Docker Images

Build the necessary Docker images for AdPart:

1. Build the base image and master image:

```bash
cd /home/ubuntu/RDF_BENCH/AdPart/
docker compose build base_image
docker compose build master
```

2. Build the images on all worker nodes:

```bash
parallel-ssh -i -h hosts.txt "cd /home/ubuntu/RDF_BENCH/AdPart/;docker compose build base_image"
parallel-ssh -i -h hosts.txt "cd /home/ubuntu/RDF_BENCH/AdPart/;docker compose build worker"
```

### Deployment with Docker Swarm

1. Remove the existing stack if necessary:

```bash
docker stack rm adpart
```

2. Deploy the `adpart` stack using the `docker-compose.yml` file:

```bash
docker stack deploy -c docker-compose.yml adpart
```

3. Check the status of the services:

```bash
docker stack services adpart
```

### Accessing the Master Node and Launching the Application

1. Connect to the master node via SSH (replace `[PORT]` and `[IP_ADDRESS]` with the appropriate values):

```bash
ssh -X -p [PORT] root@[IP_ADDRESS]
```

2. Launch the AdPart application:

```bash
cd AdPart
./Release/mgmt &
```

Follow these instructions to configure and run AdPart in your Docker Swarm environment. Make sure to replace the placeholder values with your own configurations.
