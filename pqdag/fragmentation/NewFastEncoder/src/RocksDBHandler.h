// RocksDBHandler.h
#ifndef ROCKSDBHANDLER_H
#define ROCKSDBHANDLER_H

#include <rocksdb/db.h>
#include <string>

class RocksDBHandler {
public:
    RocksDBHandler(const std::string& dbPath);
    ~RocksDBHandler();

    bool openDB();
    void closeDB();
    bool putValue(const std::string& key, const std::string& value);
    std::string getValue(const std::string& key);

private:
    std::string dbPath;
    rocksdb::DB* db;
};

#endif // ROCKSDBHANDLER_H
