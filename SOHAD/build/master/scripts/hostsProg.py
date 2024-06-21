import subprocess
import os

# Chemin du fichier contenant les noms des workers
workers_file = "workers"

# Chemin du fichier hosts temporaire
temp_hosts = "/tmp/hosts."+str(os.getpid())

# Copier le contenu actuel de /etc/hosts dans un fichier temporaire
subprocess.call(['cp', '/etc/hosts', temp_hosts])

with open(workers_file, 'r') as file:
    for worker in file:
        worker = worker.strip()
        if worker:
            # Exécution de la commande SSH pour récupérer l'adresse IP
            try:
                result = subprocess.Popen(['ssh', worker, 'ifconfig eth0 | grep "inet " | awk \'{ print $2 }\' | cut -d":" -f2'],
                                        stdout=subprocess.PIPE, universal_newlines=True)
                stdout,stderr = result.communicate()#stdout,stderr = result.communicate()ip_address = stdout.strip()print(ip_address)
                ip_address = stdout.strip()#stdout,stderr = result.communicate()ip_address = stdout.strip()print(ip_address)
                print(ip_address)#stdout,stderr = result.communicate()ip_address = stdout.strip()print(ip_address)
                if ip_address:
                    # Vérification si le worker est déjà dans temp_hosts
                    with open(temp_hosts, 'r+') as hosts:
                        if worker not in hosts.read():
                            hosts.write(str(ip_address)+" "+str(worker)+"\n")
            except subprocess.CalledProcessError:
                print("Adresse IP non trouvée pour"+str(worker))

# Remplacer /etc/hosts par le fichier temporaire mis à jour
subprocess.call(['cp', temp_hosts, '/etc/hosts'])
os.remove(temp_hosts)

print("Fichier /etc/hosts mis  jour.")

# Propagation de /etc/hosts vers les workers
with open(workers_file, 'r') as file:
    for worker in file:
        worker = worker.strip()
        if worker:
            #print(f"Propagation de /etc/hosts vers {worker}...")
            try:
                # Copie de /etc/hosts vers le worker
                subprocess.call(['scp', '/etc/hosts', str(worker)+":/tmp/hosts.tmp"])
                # Mise à jour de /etc/hosts sur le worker
                subprocess.call(['ssh', '-n', worker, 'cp /tmp/hosts.tmp /etc/hosts && rm /tmp/hosts.tmp'])
            except subprocess.CalledProcessError:
                print("Erreur lors de la propagation vers "+str(worker))
