#!/bin/sh

# Chemin du fichier contenant les noms des workers
workers_file="workers"

# Chemin du fichier hosts temporaire
temp_hosts="/tmp/hosts.$$"

# Copier le contenu actuel de /etc/hosts dans un fichier temporaire
cp /etc/hosts "$temp_hosts"

while IFS= read -r worker || [ -n "$worker" ]; do
  if [ ! -z "$worker" ]; then
    ip_address=$(nslookup "$worker" | awk '/^Address: / { print $2; exit }')
    if [ ! -z "$ip_address" ]; then
      grep -q "$worker" "$temp_hosts" || echo "$ip_address $worker" >> "$temp_hosts"
    else
      echo "Adresse IP non trouvée pour $worker"
    fi
  fi
done < "$workers_file"

cp "$temp_hosts" /etc/hosts && rm "$temp_hosts"

echo "Fichier /etc/hosts mis à jour."

while IFS= read -r worker || [ -n "$worker" ]; do
  if [ ! -z "$worker" ]; then
    echo "Propagation de /etc/hosts vers $worker..."
    scp /etc/hosts "${worker}:/tmp/hosts.tmp" && ssh -n "${worker}" 'cp /tmp/hosts.tmp /etc/hosts && rm /tmp/hosts.tmp'
  fi
done < "$workers_file"

echo "Propagation terminée."
