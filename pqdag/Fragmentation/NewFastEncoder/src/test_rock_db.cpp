#include <rocksdb/db.h>
#include <iostream>

int main() {
    rocksdb::DB* db;
    rocksdb::Options options;
    options.create_if_missing = true;
    rocksdb::Status status = rocksdb::DB::Open(options, "/tmp/testdb", &db);
    if (!status.ok()) {
        std::cerr << status.ToString() << std::endl;
        return 1;
    }
    delete db;
    std::cout << "RocksDB is working!" << std::endl;
    return 0;
}
