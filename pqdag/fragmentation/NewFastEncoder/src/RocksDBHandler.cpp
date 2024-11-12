// RocksDBHandler.cpp
#include "RocksDBHandler.h"
#include <iostream>

RocksDBHandler::RocksDBHandler(const std::string& dbPath) : dbPath(dbPath), db(nullptr) {}

RocksDBHandler::~RocksDBHandler() {
    closeDB();
}

/*bool RocksDBHandler::openDB() {
    if (db != nullptr) {
        return true; // Database is already open
    }
    
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, dbPath, &db);
    if (!status.ok()) {
        std::cerr << "Error opening RocksDB: " << status.ToString() << std::endl;
        db = nullptr;
        return false;
    }
    return true;
}*/
bool RocksDBHandler::openDB() {
    if (db != nullptr) {
        return true; // Database is already open
    }
    
    rocksdb::Options options;
    // Remove options that are not applicable in read-only mode
    rocksdb::Status status = rocksdb::DB::OpenForReadOnly(options, dbPath, &db);
    if (!status.ok()) {
        std::cerr << "Error opening RocksDB in read-only mode: " << status.ToString() << std::endl;
        db = nullptr;
        return false;
    }
    return true;
}

void RocksDBHandler::closeDB() {
    if (db != nullptr) {
        delete db;
        db = nullptr;
    }
}

bool RocksDBHandler::putValue(const std::string& key, const std::string& value) {
    if (!openDB()) return false;

    rocksdb::Status status = db->Put(rocksdb::WriteOptions(), key, value);
    if (!status.ok()) {
        std::cerr << "Error writing to RocksDB: " << status.ToString() << std::endl;
        return false;
    }
    return true;
}

std::string RocksDBHandler::getValue(const std::string& key) {
    if (!openDB()) return "";

    std::string value;
    rocksdb::Status status = db->Get(rocksdb::ReadOptions(), key, &value);
    if (!status.ok()) {
        //std::cerr << "Key not found or error retrieving key: " << status.ToString() << std::endl;
        
        return "0"; // Return empty string if key is not found
    }
    return value;
}
