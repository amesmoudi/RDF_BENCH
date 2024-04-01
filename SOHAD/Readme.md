
# S2RDF Deployment using Spark and Docker

This document outlines the steps required to deploy the S2RDF environment using Spark and Docker within a Docker Swarm setup.

## Swarm Setup Requirements

- Docker and Docker Compose must be installed on all machines.
- A `hosts.txt` file is required, containing the IP addresses of all worker nodes in the cluster.
- The `/data` folder on the master node should contain the raw data (nt files and queries).
- It is assumed that the worker nodes are named `worker01`, `worker02`, etc.
- The master node is referred to as `bench-master`. If you wish to change this name, you must also modify the `generate_compose.py` script accordingly.

### Step 1: Create a Storage Directory

Create a `/hdfs` directory on all machines to store data. Mount an external volume if necessary:

```bash
sudo mkdir /hdfs
sudo chown ubuntu /hdfs
parallel-ssh -i -h hosts.txt "sudo mkdir /hdfs"
parallel-ssh -i -h hosts.txt "sudo chown ubuntu /hdfs"
```

### Step 2: Initialize Docker Swarm

On the master node, initialize Docker Swarm:

```bash
docker swarm init
```

### Step 3: Join Swarm Cluster

Use the generated token to join other nodes to the Swarm cluster:

```bash
parallel-ssh -i -h hosts.txt "docker swarm join --token [SWARM_TOKEN] [MASTER_IP]:2377"
```
Replace `[SWARM_TOKEN]` with the actual token and `[MASTER_IP]` with the IP address of the master node.

### Step 4: Verify Cluster Nodes

Check that all nodes have successfully joined the cluster:

```bash
docker node list
```

## S2RDF Deployment Steps

### Repository Setup

Remove any existing `RDF_BENCH` directory and clone the repository:

```bash
sudo rm -r /home/ubuntu/RDF_BENCH
git clone git@github.com:amesmoudi/RDF_BENCH.git
```

Execute the following commands on all worker nodes:

```bash
parallel-ssh -i -h hosts.txt "sudo rm -r /home/ubuntu/RDF_BENCH"
parallel-ssh -i -h hosts.txt "git clone git@github.com:amesmoudi/RDF_BENCH.git"
```

### Build Images

On the master node:

```bash
cd ~/RDF_BENCH
sh SOHAD/build-master-image.sh > build-master.log &
tail -f build-master.log 
```

On worker nodes, build the worker images:

```bash
parallel-ssh -i -h hosts.txt "nohup sh ./RDF_BENCH/SOHAD/build-worker-image.sh > build-worker.log 2>&1 &"
parallel-ssh -i -h hosts.txt "tail -f build-worker.log"
```

### Modify the `nbworkers` Parameter and Deploy Cluster

```bash
sh deploy_cluster.sh 
```

### Load Data

To load data, execute:

```bash
sh loadData.sh /data/watdiv10M.nt 0.25
```
The `loadData.sh` script takes the path of the NT file as a parameter.

### Run Queries

Execute SPARQL queries with:

```bash
sh runWatdivQueries.sh -q /data/queries/watdiv/watdiv10M/ -d watdiv10M -s 0.25
```
- `-q` allows to specify the SPARQL query folder 
- `-d` the name of the database
- `-s` to specify sUB
