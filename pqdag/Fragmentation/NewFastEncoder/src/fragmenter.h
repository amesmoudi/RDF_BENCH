#ifndef FRAGMENTER_H
#define FRAGMENTER_H

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include <tuple>
#include <rocksdb/db.h>
// Définition du hachage personnalisé pour std::set<int>
struct SetHash {
    std::size_t operator()(const std::set<int>& s) const {
        std::size_t hash = 0;
        for (const int& i : s) {
            hash ^= std::hash<int>{}(i) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
};

class Fragmenter {
public:
    void processFiles(const std::string& inputFilePath1, const std::string& inputFilePath2);

private:
    void processFile(const std::string& inputFilePath, const std::string& fragmentPrefix, const std::string& metadataFileName, std::unordered_map<int, int>& nodeFragmentMap, std::ofstream& nodeRefFile,rocksdb::DB* db);
    void writeMetadataToFile(const std::string& directory, const std::string& fileName, const std::string& fragmentPrefix);
    void flushNodeFragmentMapToFile(std::unordered_map<int, int>& nodeFragmentMap, std::ofstream& nodeRefFile,rocksdb::DB* db);
    int generateFragmentID();

    std::unordered_map<std::set<int>, int, SetHash> characteristicSetToFragmentID;
    std::unordered_map<int, std::set<int>> characteristicSets;
    std::unordered_map<int, std::pair<int, int>> fragments_stats;//fragment_id => (ds numbers, triples number)
    std::unordered_map<int, std::ofstream> fragmentFiles;
    std::unordered_map<int, int> fragments_triples;
    std::unordered_map<int, int> fragments_ds;
    int currentID = 1;  // Pour suivre l'ID de fragment global
    const size_t MAX_MAP_SIZE = 1000;  // Taille maximale des maps avant vidage sur disque
};

#endif // FRAGMENTER_H
