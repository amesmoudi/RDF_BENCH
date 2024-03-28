# Variables pour le chemin complet du fichier, le nom du dataset, et la base du nom de fichier
filePath=$1
datasetName=$(basename $(dirname $filePath))
fileName=$(basename $filePath)
fileNameBase=${fileName%.nt}

# Création du dossier cible pour les fichiers traités
mkdir -p /data/tmp/$datasetName/$fileNameBase/

# Traitement du fichier NT
python3 replaceNS.py $filePath /data/tmp/$datasetName/$fileNameBase/$fileName

# Chargement du fichier traité dans HDFS (ajuster selon le besoin réel)
hdfs dfs -mkdir -p /user/root/$datasetName/$fileNameBase/
hdfs dfs -put /data/tmp/$datasetName/$fileNameBase/$fileName /user/root/$datasetName/$fileNameBase/

# Lancement du partitionneur de S2RDF
# Note : Assurez-vous que les chemins et options sont corrects pour votre environnement et utilisation spécifiques
python2 /opt/workspace/DataSetCreator/DataSetCreator.py -i /user/root/$datasetName/$fileNameBase/$fileName -s 0.25
mkdir -p /data/tmp/$datasetName/$fileNameBase/stats
mv stat-* /data/tmp/$datasetName/$fileNameBase/stats/
