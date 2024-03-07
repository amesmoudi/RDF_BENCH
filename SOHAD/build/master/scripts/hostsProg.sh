!/bin/bash

# Fichier contenant la liste des travailleurs
WORKER_LIST="workers"

# Fichier temporaire pour les entrées de /etc/hosts
TEMP_HOSTS="/tmp/hosts"

# Créer/clear le fichier temporaire
> $TEMP_HOSTS

# Boucle sur chaque travailleur pour récupérer les entrées et les ajouter à /etc/hosts
while read worker; do
    echo "Récupération des entrées de $worker..."
    ssh $worker "cat /etc/hosts" | grep 'worker' >> $TEMP_HOSTS
done < $WORKER_LIST

# Supprimer les entrées dupliquées pour éviter les problèmes
sort $TEMP_HOSTS | uniq > $TEMP_HOSTS.tmp
mv $TEMP_HOSTS.tmp $TEMP_HOSTS

# Ajouter les nouvelles entrées au fichier /etc/hosts local
echo "Mise à jour du fichier /etc/hosts local..."
cat $TEMP_HOSTS | tee -a /etc/hosts

# Propager le fichier /etc/hosts mis à jour à tous les travailleurs
while read worker; do
    echo "Propagation du fichier /etc/hosts à $worker..."
    scp $TEMP_HOSTS $worker:/etc/hosts
done < $WORKER_LIST

echo "Mise à jour et propagation terminées."