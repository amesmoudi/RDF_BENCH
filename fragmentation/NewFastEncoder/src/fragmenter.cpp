#include "fragmenter.h"
#include <fstream>
#include <sstream>
#include <tuple>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <filesystem>
#include <rocksdb/db.h>

namespace fs = std::filesystem;

void Fragmenter::processFiles(const std::string& inputFilePath1, const std::string& inputFilePath2) {
    std::unordered_map<int, int> spoNodeFragmentMap;
    std::unordered_map<int, int> opsNodeFragmentMap;
    std::string directory = fs::path(inputFilePath1).parent_path().string();
    // Initialize RocksDB options and open the database
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::DB* spoDB;
    rocksdb::DB* opsDB;
    rocksdb::Status spoStatus = rocksdb::DB::Open(options, directory + "/node_fragment_spo", &spoDB);
    rocksdb::Status opsStatus = rocksdb::DB::Open(options, directory + "/node_fragment_ops", &opsDB);
    if (!spoStatus.ok() || !opsStatus.ok()) {
        std::cerr << "Error opening RocksDB: " 
                  << (spoStatus.ok() ? opsStatus.ToString() : spoStatus.ToString()) 
                  << std::endl;
        return;
    }
    // Ouvrir les fichiers pour stocker les références des nœuds
    std::ofstream spoNodeRefFile(directory + "/spo_references.txt");
    std::ofstream opsNodeRefFile(directory + "/ops_references.txt");

    processFile(inputFilePath1, "spo_fg", "spo_index.txt", spoNodeFragmentMap, spoNodeRefFile,spoDB);
    // Vider les maps restantes dans les fichiers de référence
    flushNodeFragmentMapToFile(spoNodeFragmentMap, spoNodeRefFile,spoDB);
    //writeMetadataToFile(directory, "spo_metadata.txt", "spo_fg");

    // Réinitialiser les structures spécifiques aux fichiers
    characteristicSetToFragmentID.clear();
    characteristicSets.clear();
    fragmentFiles.clear();

    processFile(inputFilePath2, "ops_fg", "ops_index.txt", opsNodeFragmentMap, opsNodeRefFile,opsDB);
    // Vider les maps restantes dans les fichiers de référence
    flushNodeFragmentMapToFile(opsNodeFragmentMap, opsNodeRefFile,opsDB);
    //writeMetadataToFile(directory, "ops_metadata.txt", "ops_fg");

    spoNodeRefFile.close();
    opsNodeRefFile.close();
    delete spoDB;
    delete opsDB;
}


void Fragmenter::processFile(const std::string& inputFilePath, const std::string& fragmentPrefix, const std::string& metadataFileName, 
std::unordered_map<int, int>& nodeFragmentMap, std::ofstream& nodeRefFile,rocksdb::DB* db) {
    std::ifstream file(inputFilePath);
    std::string line;
    // Extraire le répertoire du fichier d'entrée
    std::string directory = fs::path(inputFilePath).parent_path().string();
    // Generate the set of characteristics set
    while (getline(file, line)) {
        std::istringstream iss(line);
        int s, p, o;
        if (!(iss >> s >> p >> o)) { break; }
        characteristicSets[s].insert(p);
    }
    file.close();

    // Initialize the fragment's files 
    for (const auto& pair : characteristicSets) {
        const std::set<int>& characteristicSet = pair.second;
        // Si un fragment pour cet ensemble de caractéristiques n'existe pas encore, le créer
        if (characteristicSetToFragmentID.find(characteristicSet) == characteristicSetToFragmentID.end()) {
            int id = generateFragmentID();
            characteristicSetToFragmentID[characteristicSet] = id;
            fragmentFiles[id].open(directory + "/" + fragmentPrefix + "_" + std::to_string(id) + ".txt", std::ios::out);
        }
    }

    // Lire les lignes du fichier d'entrée à nouveau et écrire dans les fichiers de fragments appropriés
    file.open(inputFilePath);
    int last_s = -1;
    while (getline(file, line)) {
        std::istringstream iss(line);
        int s, p, o;
        if (!(iss >> s >> p >> o)) { break; }
        const std::set<int>& characteristicSet = characteristicSets[s];
        int fragmentID = characteristicSetToFragmentID[characteristicSet];
        //store the information about the number of datastars
        if(last_s != s){
           fragments_ds[fragmentID]++;
           last_s = s;
        }
        //stores the information about the number of triples
        fragments_triples[fragmentID]++;
        fragmentFiles[fragmentID] << s << " " << p << " " << o << "\n";

        // Mettre à jour les fragments du nœud
        nodeFragmentMap[s] = fragmentID;
        // Si la taille de la map dépasse la limite, écrire sur disque
        if (nodeFragmentMap.size() >= MAX_MAP_SIZE) 
            flushNodeFragmentMapToFile(nodeFragmentMap, nodeRefFile,db);
    }
    file.close();

    // Fermer tous les fichiers de fragments
    for (auto& pair : fragmentFiles) 
        pair.second.close();
    writeMetadataToFile(directory, metadataFileName, fragmentPrefix);
}


/**
 * Write Characteristics Set (separated by tab) ; Fragment Id
*/
void Fragmenter::writeMetadataToFile(const std::string& directory, const std::string& fileName, const std::string& fragmentPrefix) {
    std::ofstream metadataFile(directory + "/" + fileName);
    for (const auto& pair : characteristicSetToFragmentID) {
        const std::set<int>& characteristicSet = pair.first;
        int fragmentID = pair.second;

        // Écrire les éléments de l'ensemble de caractéristiques séparés par des tabulations
        for (auto it = characteristicSet.begin(); it != characteristicSet.end(); ++it) {
            if (it != characteristicSet.begin()) {
                metadataFile << "\t";
            }
            metadataFile << *it;
        }
        // Ajouter le point-virgule et l'ID de fragment à la fin
        metadataFile << ";" << fragmentID << "\n";
    }
    metadataFile.close();
}


void Fragmenter::flushNodeFragmentMapToFile(std::unordered_map<int, int>& nodeFragmentMap, std::ofstream& nodeRefFile,rocksdb::DB* db) {
    rocksdb::WriteBatch batch;
    for (const auto& pair : nodeFragmentMap) {
        int node = pair.first;
        int fragmentID = pair.second;
        nodeRefFile << node << " " << fragmentID << "\n";
        std::string key = std::to_string(node);
        std::string value = std::to_string(fragmentID);
        batch.Put(key, value);
    }
    rocksdb::Status status = db->Write(rocksdb::WriteOptions(), &batch);
    if (!status.ok()) 
        std::cerr << "Error writing to RocksDB: " << status.ToString() << std::endl;
    nodeFragmentMap.clear();  // Vider la map après avoir écrit sur disque
}

int Fragmenter::generateFragmentID() {
    return currentID++;
}
