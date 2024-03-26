import subprocess
import os

# Chemin du fichier contenant les noms des workers
workers_file = "workers"

# Chemin du fichier hosts temporaire
temp_hosts = f"/tmp/hosts.{os.getpid()}"

# Copier le contenu actuel de /etc/hosts dans un fichier temporaire
subprocess.run(['cp', '/etc/hosts', temp_hosts], check=True)

with open(workers_file, 'r') as file:
    for worker in file:
        worker = worker.strip()
        if worker:
            # Exécution de la commande SSH pour récupérer l'adresse IP
            try:
                result = subprocess.run(['ssh', worker, 'ifconfig eth0 | grep "inet " | awk \'{ print $2 }\' | cut -d":" -f2'],
                                        check=True, stdout=subprocess.PIPE, universal_newlines=True)
                ip_address = result.stdout.strip()
                if ip_address:
                    # Vérification si le worker est déjà dans temp_hosts
                    with open(temp_hosts, 'r+') as hosts:
                        if worker not in hosts.read():
                            hosts.write(f"{ip_address} {worker}\n")
            except subprocess.CalledProcessError:
                print(f"Adresse IP non trouvée pour {worker}")

# Remplacer /etc/hosts par le fichier temporaire mis à jour
subprocess.run(['cp', temp_hosts, '/etc/hosts'], check=True)
os.remove(temp_hosts)

print("Fichier /etc/hosts mis à jour.")

# Propagation de /etc/hosts vers les workers
with open(workers_file, 'r') as file:
    for worker in file:
        worker = worker.strip()
        if worker:
            print(f"Propagation de /etc/hosts vers {worker}...")
            try:
                # Copie de /etc/hosts vers le worker
                subprocess.run(['scp', '/etc/hosts', f"{worker}:/tmp/hosts.tmp"], check=True)
                # Mise à jour de /etc/hosts sur le worker
                subprocess.run(['ssh', '-n', worker, 'cp /tmp/hosts.tmp /etc/hosts && rm /tmp/hosts.tmp'], check=True)
            except subprocess.CalledProcessError:
                print(f"Erreur lors de la propagation vers {worker}.")

print("Propagation terminée.")
