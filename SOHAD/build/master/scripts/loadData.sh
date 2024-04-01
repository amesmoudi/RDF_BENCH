# Variables pour le chemin complet du fichier, le nom du dataset, et la base du nom de fichier
filePath=$1
datasetName=$(basename $(dirname $filePath))
fileName=$(basename $filePath)
fileNameBase=${fileName%.nt}
sUB=$2

# Création du dossier cible pour les fichiers traités
# Define the folder path
folderPath="/data/tmp/$datasetName/$fileNameBase"

# Check if the folder exists and remove it
if [ -d "$folderPath" ]; then
    echo "Removing directory: $folderPath"
    rm -rf "$folderPath"
    hds dfs -r /user/root/$datasetName/$fileNameBase/
else
    echo "Directory $folderPath does not exist."
fi

# You can then proceed with creating the directory if needed
mkdir -p "$folderPath"

# Traitement du fichier NT
python3 replaceNS.py $filePath /data/tmp/$datasetName/$fileNameBase/$fileName

# Chargement du fichier traité dans HDFS (ajuster selon le besoin réel)
hdfs dfs -mkdir -p /user/root/$datasetName/$fileNameBase/
hdfs dfs -put /data/tmp/$datasetName/$fileNameBase/$fileName /user/root/$datasetName/$fileNameBase/

# Lancement du partitionneur de S2RDF
python2 /opt/workspace/DataSetCreator/DataSetCreator.py -i /user/root/$datasetName/$fileNameBase/$fileName -s $sUB
mkdir -p /data/tmp/$datasetName/$fileNameBase/stats
mv stat_* /data/tmp/$datasetName/$fileNameBase/stats/
mv *.log /data/tmp/$datasetName/$fileNameBase/stats/
mv *.err /data/tmp/$datasetName/$fileNameBase/stats/

