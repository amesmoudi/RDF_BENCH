#include "DictionaryIndexer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <vector>
#include <rocksdb/db.h>
namespace fs = std::filesystem;

/**
 * Reverse the dictionnary file Node => id to [id => Node]
*/
void DictionaryIndexer::reverseDictionaryFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::string line;
    std::string directory = fs::path(filePath).parent_path().string();
    // Set up RocksDB options
    rocksdb::Options options;
    options.create_if_missing = true;
    // Open the database
    rocksdb::DB* idNodeDB;
    rocksdb::Status idNodeDBStatus = rocksdb::DB::Open(options, directory + "/id_node", &idNodeDB);
    if (!idNodeDBStatus.ok()) {
        std::cerr << "Error opening RocksDB: " << idNodeDBStatus.ToString() << std::endl;
        return;
    }
    // Prepare for batching
    rocksdb::WriteBatch batch;
    const size_t BATCH_SIZE = 1000;  // Adjust this value as needed
    size_t count = 0;
    // Read and process the file line by line
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int id;
        std::string word;
        // Parse the line
        if (!(iss >> id >> std::ws)) 
            continue;  // Skip invalid lines
        std::getline(iss, word);
        // Add to batch
        std::string key = std::to_string(id);
        batch.Put(key, word);
        count++;
        // Write batch to DB when batch size is reached
        if (count % BATCH_SIZE == 0) {
            rocksdb::Status s = idNodeDB->Write(rocksdb::WriteOptions(), &batch);
            if (!s.ok()) 
                std::cerr << "Error writing batch to RocksDB: " << s.ToString() << std::endl;
            batch.Clear();  // Clear batch for next set of operations
        }
    }
    // Write any remaining entries in the batch
    if (count % BATCH_SIZE != 0) {
        rocksdb::Status s = idNodeDB->Write(rocksdb::WriteOptions(), &batch);
        if (!s.ok()) 
            std::cerr << "Error writing final batch to RocksDB: " << s.ToString() << std::endl;
    }
    file.close();
    delete idNodeDB;
}


