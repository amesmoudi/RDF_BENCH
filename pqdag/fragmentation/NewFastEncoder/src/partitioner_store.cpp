#include "partitioner_store.h"
#include "utils.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <unordered_map>
#include <regex>
#include <filesystem>
#include <cctype>
#include <limits>
#include <rocksdb/write_batch.h>
namespace fs = std::filesystem;



partitioner_store::partitioner_store() {
    rocksdb::Options options;
    options.create_if_missing = true;
    // Open subjects/objects database
    rocksdb::Status status = rocksdb::DB::Open(options, "/bindata/so_db", &so_db);
    if (!status.ok()) {
        std::cerr << "Unable to open subjects/objects DB: " << status.ToString() << std::endl;
        return;
    }
    // Open predicates database
    status = rocksdb::DB::Open(options, "/bindata/predicates_db", &predicates_db);
    if (!status.ok()) {
        std::cerr << "Unable to open predicates DB: " << status.ToString() << std::endl;
        return;
    }
}

partitioner_store::~partitioner_store() {}

int getdir(string dir, vector<string> &files) {
    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(dir.c_str())) == NULL) {
        cout << "Error(" << errno << ") opening " << dir << endl;
        return errno;
    }
    while ((dirp = readdir(dp)) != NULL) {
        if (((int)dirp->d_type) == 8)
            files.push_back(string(dirp->d_name));
    }
    closedir(dp);
    return 0;
}

string partitioner_store::infer_literal_type(const string& literal) {
    bool is_integer = true;
    bool is_float = false;
    bool has_decimal = false;
    bool has_exponent = false;
    int start = 0;
    // Handle optional sign
    if (literal[start] == '-' || literal[start] == '+') 
        start++;

    for (int i = start; i < literal.size(); i++) {
        char c = literal[i];
        if (std::isdigit(c)) 
            continue;
        else if (c == '.' && !has_decimal) {
            // Floating point number (only allow one decimal point)
            has_decimal = true;
            is_integer = false;
        } else if ((c == 'e' || c == 'E') && !has_exponent) {
            // Exponential notation
            is_float = true;
            has_exponent = true;
            is_integer = false;
            // Handle optional sign after exponent
            if (i + 1 < literal.size() && (literal[i + 1] == '-' || literal[i + 1] == '+')) 
                i++;  // Skip the sign
        } else // If it's not a digit, decimal point, or exponent sign, it's not a number
            return "String";
    }

    if (is_integer) {
        try {
            long long value = std::stoll(literal);
            if (value >= std::numeric_limits<short>::min() && value <= std::numeric_limits<short>::max()) 
                return "Short";
            else if (value >= std::numeric_limits<int>::min() && value <= std::numeric_limits<int>::max())
                return "Integer";
            else 
                return "Long";
            
        } catch (...) {
            return "String";
        }
    }
    if (has_decimal || is_float) {
        try {
            double value = std::stod(literal);
            if (value >= std::numeric_limits<float>::min() && value <= std::numeric_limits<float>::max()) 
                return "Float";
            else 
                return "Double";
        } catch (...) {
            return "String";
        }
    }
    return "String";
}

void partitioner_store::infer_schema(const string& predicate, const string& subject_type, const string& object_type) {
    const string& subj_type = (subject_type == "Mixed") ? "String" : subject_type;
    const string& obj_type = (object_type == "Mixed") ? "String" : object_type;
    auto result = schema.emplace(predicate, std::make_pair(subj_type, obj_type));
    if (!result.second) {
        auto& types = result.first->second;
        // Update subject type if there's a conflict
        if (types.first != subj_type && types.first != "String") 
            types.first = "String";
        // Update object type if there's a conflict
        if (types.second != obj_type && types.second != "String") 
            types.second = "String";
    }
}

void partitioner_store::load_encode_rdf_data(string input_dir, string output_file_name) {
    print_to_screen(part_string);
    Profiler profiler;
    profiler.startTimer("load_rdf_data");
    ll num_rec = 0;
    ofstream ofs;
    string input_file;
    Type::ID objectType;
    ll sid, pid, oid;
    string subject, predicate, object, objectSubType;
    vector<string> files;
    ll so_id = 1;
    ll predicate_id = 1;
    rocksdb::WriteBatch so_write_batch;
    rocksdb::WriteBatch predicates_write_batch;
    // Define a batch size (number of operations before writing to DB)
    const size_t BATCH_SIZE = 10000000;
    size_t so_batch_count = 0;
    size_t predicates_batch_count = 0;
    // Create caches for subjects/objects and predicates
    unordered_map<string, ll> so_cache;
    unordered_map<string, ll> predicates_cache;

    getdir(input_dir, files);
    ofs.open(output_file_name.c_str(), ofstream::out);
    cout << "Output file name: " << output_file_name << endl;
    if (!ofs) {
        cerr << "Failed to open the output file." << endl;
        return;
    }
    for (unsigned int file_id = 0; file_id < files.size(); file_id++) {
        input_file = input_dir + "/" + files[file_id];
        ifstream fin(input_file.c_str());
        TurtleParser parser(fin);
        print_to_screen("Reading triples from file: " + input_file);
        try {
            while (true) {
                try {
                    if (!parser.parse(subject, predicate, object, objectType, objectSubType))
                        break;
                } catch (const TurtleParser::Exception& e) {
                    cerr << e.message << endl;
                    while (fin.get() != '\n');
                    continue;
                }
                num_rec++;
                // --- Subject Handling ---
                auto so_cache_it = so_cache.find(subject);
                if (so_cache_it != so_cache.end()) {
                    // Subject found in cache
                    sid = so_cache_it->second;
                } else {
                    // Subject not in cache, check database
                    std::string sid_str;
                    rocksdb::Status s = so_db->Get(rocksdb::ReadOptions(), subject, &sid_str);
                    if (s.ok()) 
                        sid = std::stoll(sid_str);
                    else {
                        // Subject not found, assign new ID
                        sid = so_id++;
                        sid_str = std::to_string(sid);
                        // Add to batch
                        so_write_batch.Put(subject, sid_str);
                        so_batch_count++;
                    }
                    // Update cache
                    so_cache[subject] = sid;
                }

                // --- Predicate Handling ---
                auto predicates_cache_it = predicates_cache.find(predicate);
                if (predicates_cache_it != predicates_cache.end()) {
                    // Predicate found in cache
                    pid = predicates_cache_it->second;
                } else {
                    // Predicate not in cache, check database
                    std::string pid_str;
                    rocksdb::Status s = predicates_db->Get(rocksdb::ReadOptions(), predicate, &pid_str);
                    if (s.ok()) 
                        pid = std::stoll(pid_str);
                    else {
                        // Predicate not found, assign new ID
                        pid = predicate_id++;
                        pid_str = std::to_string(pid);
                        // Add to batch
                        predicates_write_batch.Put(predicate, pid_str);
                        predicates_batch_count++;
                    }
                    // Update cache
                    predicates_cache[predicate] = pid;
                }

                // --- Object Handling ---
                // Normalize object (removing newlines and carriage returns)
                for (unsigned i = 0; i < object.length(); i++)
                    if (object[i] == '\n' || object[i] == '\r')
                        object[i] = ' ';

                auto so_cache_it_obj = so_cache.find(object);
                if (so_cache_it_obj != so_cache.end()) {
                    // Object found in cache
                    oid = so_cache_it_obj->second;
                } else {
                    // Object not in cache, check database
                    std::string oid_str;
                    rocksdb::Status s = so_db->Get(rocksdb::ReadOptions(), object, &oid_str);
                    if (s.ok()) 
                        oid = std::stoll(oid_str);
                    else {
                        // Object not found, assign new ID
                        oid = so_id++;
                        oid_str = std::to_string(oid);
                        // Add to batch
                        so_write_batch.Put(object, oid_str);
                        so_batch_count++;
                    }
                    // Update cache
                    so_cache[object] = oid;
                }
                infer_schema(predicate, "String", infer_literal_type(object));

                // Write the triple to the output file
                ofs << sid << " " << pid << " " << oid << "\n";

                //Log print
                if (num_rec % BATCH_SIZE == 0)
                        cout << num_rec << endl;
                if (so_batch_count >= BATCH_SIZE) {
                    // Write the batch to RocksDB
                    rocksdb::Status s = so_db->Write(rocksdb::WriteOptions(), &so_write_batch);
                    if (!s.ok()) {
                        std::cerr << "Error writing SO batch to RocksDB: " << s.ToString() << std::endl;
                    }
                    so_write_batch.Clear();
                    so_batch_count = 0;
                    so_cache.clear();
                }
                if (predicates_batch_count >= BATCH_SIZE) {
                    rocksdb::Status s = predicates_db->Write(rocksdb::WriteOptions(), &predicates_write_batch);
                    if (!s.ok()) {
                        std::cerr << "Error writing predicates batch to RocksDB: " << s.ToString() << std::endl;
                    }
                    predicates_write_batch.Clear();
                    predicates_batch_count = 0;
                    predicates_cache.clear();
                }
            }
            fin.close();
        } catch (const TurtleParser::Exception&) {
            return;
        }
    }
    // Write any remaining entries in batches and clear caches
    if (so_batch_count > 0) {
        rocksdb::Status s = so_db->Write(rocksdb::WriteOptions(), &so_write_batch);
        if (!s.ok()) 
            std::cerr << "Error writing final SO batch to RocksDB: " << s.ToString() << std::endl;
        so_write_batch.Clear();
        so_cache.clear();
    }

    if (predicates_batch_count > 0) {
        rocksdb::Status s = predicates_db->Write(rocksdb::WriteOptions(), &predicates_write_batch);
        if (!s.ok()) 
            std::cerr << "Error writing final predicates batch to RocksDB: " << s.ToString() << std::endl;
        predicates_write_batch.Clear();
        predicates_cache.clear();
    }

    ofs.close();
    total_data_size = num_rec;
    print_to_screen(part_string + "\nTotal number of triples: " + toString(this->total_data_size) + " records");
    profiler.pauseTimer("load_rdf_data");
    print_to_screen("Done with data encoding in " + toString(profiler.readPeriod("load_rdf_data")) + " sec");
    profiler.clearTimer("load_rdf_data");
    fs::path output_path(output_file_name);
    string folder = output_path.parent_path().string();
    cout << "Folder: " << folder << endl;
    this->dump_dictionaries(folder);
    this->dump_schema(folder + "/schema.txt");
}


void partitioner_store::dump_dictionaries(string folder) {
    print_to_screen(part_string);
    Profiler profiler;
    profiler.startTimer("dump_dictionaries");
    print_to_screen("Dumping Dictionaries!");
    // Dump subjects/objects from RocksDB
    dump_rocksdb(so_db, folder + "/nodes.dic");
    // Dump predicates from RocksDB
    dump_rocksdb(predicates_db, folder + "/predicate.dic");
    profiler.pauseTimer("dump_dictionaries");
    print_to_screen("Done with dumping dictionaries in " + toString(profiler.readPeriod("dump_dictionaries")) + " sec");
    profiler.clearTimer("dump_dictionaries");
}

void partitioner_store::dump_rocksdb(rocksdb::DB* db, const std::string& file_name) {
    ofstream ofs(file_name.c_str());
    if (!ofs) {
        cerr << "Failed to open file: " << file_name << endl;
        return;
    }
    rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        std::string key = it->key().ToString();
        std::string value = it->value().ToString();
        // Write key-value pair to the file (e.g., "ID Term")
        ofs << value << "\t" << key << "\n";
    }
    if (!it->status().ok()) 
        cerr << "An error occurred during iteration: " << it->status().ToString() << endl;
    delete it;
    ofs.close();
}

void partitioner_store::dump_schema(string file_name) {
    ofstream ofs(file_name.c_str());
    if (!ofs) {
        cerr << "Failed to open the schema file." << endl;
        return;
    }
    // Iterate over the schema (which is in memory)
    for (const auto& entry : schema) {
        const string& predicate = entry.first;
        const string& subj_type = entry.second.first;
        const string& obj_type = entry.second.second;
        // Retrieve the predicate ID from RocksDB
        std::string predicate_id_str;
        rocksdb::Status s = predicates_db->Get(rocksdb::ReadOptions(), predicate, &predicate_id_str);
        if (s.ok()) {
            ll predicate_id = std::stoll(predicate_id_str);
            ofs << predicate_id << "\t" << "<"<<predicate<<">" << "\t" << subj_type << "\t" << obj_type << "\n";
        } else {
            cerr << "Predicate " << predicate << " not found in RocksDB." << endl;
        }
    }

    ofs.close();
}

