#ifndef PARTITIONER_STORE_H_
#define PARTITIONER_STORE_H_
#include "utils.h"
#include "profiler.h"
#include "row.h"
#include "TurtleParser.hpp"
#include <unordered_map>
#include <set>
#include <regex>
#include <rocksdb/db.h>
#include <rocksdb/options.h>

using namespace std;

struct triple {
    triple() {}
    triple(ll s, ll p, ll o, Type::ID _ot) {
        subject = s;
        predicate = p;
        object = o;
        ot = _ot;
    }
    ll subject;
    ll predicate;
    ll object;
    Type::ID ot;
    string print() {
        return toString(subject) + " " + toString(predicate) + " " + toString(object);
    }
};

class partitioner_store {
public:
    partitioner_store();
    virtual ~partitioner_store();
    void load_encode_rdf_data(string input_dir, string output_file_name);
    void dump_rocksdb(rocksdb::DB* db, const std::string& file_name);
    void dump_dictionaries(string file_name);
    void dump_schema(string file_name);
    ll total_data_size;
    rocksdb::DB* so_db;           
    rocksdb::DB* predicates_db;
protected:
    string infer_literal_type(const string& literal);
    void infer_schema(const string& predicate, const string& subject_type, const string& object_type);
    unordered_map<string, pair<string, string>> schema; // Schéma inféré avec types des sujets et objets
};
#endif /* PARTITIONER_STORE_H_ */
