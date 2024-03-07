import sys

# Vérifier si un argument (nombre de workers) est passé au script
if len(sys.argv) != 2:
    print("Usage: python generate_compose.py <number_of_workers>")
    sys.exit(1)

try:
    number_of_workers = int(sys.argv[1])
except ValueError:
    print("Please enter a valid integer for the number of workers.")
    sys.exit(1)

# Partie de base du fichier docker-compose.yml
compose_file = """version: '3.9'

services:
  master:
    image: sohad_master:latest
    hostname: master
    volumes:
      - /hdfs:/opt/hdfs/
    ports:
      - "2222:22"
      - "50070:50070"
      - "8088:8088"
    deploy:
      replicas: 1
      placement:
        constraints:
          - node.hostname == bench-master
    networks:
      - sohad-network
"""

# Générer les services workers en fonction du nombre spécifié
for i in range(1, number_of_workers + 1):
    worker_service = f"""
  worker{i:02d}:
    image: sohad_worker:latest
    hostname: worker{i:02d}
    volumes:
      - /hdfs:/opt/hdfs/
    deploy:
      replicas: 1
      placement:
        constraints:
          - node.hostname == worker{i:02d}
    networks:
      - sohad-network
"""
    compose_file += worker_service

# Ajouter la définition du réseau à la fin du fichier
compose_file += """
networks:
  sohad-network:
    driver: overlay
"""

# Écrire le fichier docker-compose.yml
with open('docker-compose-deploy.yml', 'w') as file:
    file.write(compose_file)

print("docker-compose-deploy.yml file has been generated successfully.")
