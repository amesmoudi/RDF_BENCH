#Transformer le fichier nt raw
mkdir -p /data/tmp/$1 $2
python3 /opt/replaceNS.py $1 /data/tmp/$1/$1

#Charger dans le HDFS 
hdfs dfs -put /data/tmp/$1/*.nt

# Lancer le partitionneur de S2RDF
mkdir /data/tmp/$1/stats
python2 /opt/workspace/DataSetCreator/S2RDF_DataSetCreator/DataSetCreator.py -i /user/root/$1 -s $2
mv stat-* /data/tmp/$1/data/tmp/$1/stats









