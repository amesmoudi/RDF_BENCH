#include "FragmentReencoder.h"
#include <fstream>
#include <omp.h>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <chrono>
#include "RocksDBHandler.h"
#include <cstdlib>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <vector> /

namespace fs = std::filesystem;



FragmentReencoder::FragmentReencoder(const std::string& inputDir, const std::string& outputDir)
    : inputDirectory(inputDir), outputDirectory(outputDir) {
    loadSchema(inputDirectory + "/schema.txt");
}


// Load schema file
void FragmentReencoder::loadSchema(const std::string& schemaFile) {
    std::ifstream file(schemaFile);
    if (!file.is_open()) {
        std::cerr << "Error opening schema file: " << schemaFile << std::endl;
        return;
    }

    std::string line;
    while (getline(file, line)) {
        std::istringstream iss(line);
        int id;
        std::string uri, type1, type2;
        if (iss >> id >> uri >> type1 >> type2) {
            schemaMap[id] = std::make_tuple(uri, type1, type2);
        }
    }

    file.close();
}





// Helper function to convert type to short representation
std::string FragmentReencoder::convertTypeToShortForm(const std::string& type) {
    if (type == "String") return "string";
    if (type == "Long") return "b";
    if (type == "Integer") return "i";
    if (type == "Short") return "t";
    if (type == "Float") return "f";
    if (type == "Double") return "d";
    return type; // Default case if type is unrecognized
}
// Helper function to check if a type is numeric
bool FragmentReencoder::isNumericType(const std::string& type) {
    return type == "Integer" || type == "Short" || type == "Long" || type == "Float" || type == "Double";
}
void FragmentReencoder::encodeFragment(const std::string& fragmentFile, RocksDBHandler& spoDB, RocksDBHandler& opsDB, RocksDBHandler& idNodeDB) {
    std::ifstream file(fragmentFile);
    if (!file.is_open()) {
        std::cerr << "Error opening fragment file: " << fragmentFile << std::endl;
        return;
    }

    // Extract fragment ID from the file name
    std::string fragmentFileName = fs::path(fragmentFile).filename().string();
    size_t firstUnderscorePos = fragmentFileName.find('_');
    size_t secondUnderscorePos = fragmentFileName.find('_', firstUnderscorePos + 1);
    size_t dotPos = fragmentFileName.rfind('.');
    std::string fragmentID = fragmentFileName.substr(secondUnderscorePos + 1, dotPos - secondUnderscorePos - 1);

    // Open output files
    std::ofstream outputFileDic(outputDirectory + "/" + fragmentID + ".dic");
    std::ofstream outputFile(outputDirectory + "/" + fragmentID + ".data");
    std::ofstream schemaFile(outputDirectory + "/" + fragmentID + ".schema");

    std::string line;
    bool isSPOFragment = (fragmentFile.find("spo") != std::string::npos);

    bool nodeTypeWritten = false;
    std::unordered_set<int> predicatesWritten;
    std::unordered_set<int> writtenNodes;

    // Add caches for database reads
    std::unordered_map<int, std::string> opsCache;
    std::unordered_map<int, std::string> spoCache;
    std::unordered_map<int, std::string> idNodeCache;

    // Define chunk size for processing
    const size_t CHUNK_SIZE = 1000000; // Adjust based on memory constraints
    std::vector<std::string> dataLines;
    dataLines.reserve(CHUNK_SIZE);

    while (getline(file, line)) {
        std::istringstream iss(line);
        int node1, predicate, node2;
        if (!(iss >> node1 >> predicate >> node2)) {
            std::cerr << "Error parsing line (expected 3 values, got fewer): " << line << std::endl;
            continue;
        }

        // Lookup the schema information for the predicate
        auto schemaIter = schemaMap.find(predicate);
        if (schemaIter == schemaMap.end()) {
            std::cerr << "Predicate " << predicate << " not found in schema map." << std::endl;
            continue;
        }

        const auto& [uri, type1, type2] = schemaIter->second;

        // Determine the types of node1 and node2 based on fragment type
        std::string typeNode1 = isSPOFragment ? type1 : type2;
        std::string typeNode2 = isSPOFragment ? type2 : type1;

        // Convert types to short form
        std::string shortTypeNode1 = convertTypeToShortForm(typeNode1);
        std::string shortTypeNode2 = convertTypeToShortForm(typeNode2);

        // Write the type of node1 only once
        if (!nodeTypeWritten) {
            schemaFile << shortTypeNode1 << std::endl;
            nodeTypeWritten = true;
        }

        // Write the predicate and type of node2 if not already written
        if (predicatesWritten.insert(predicate).second) {
            schemaFile << predicate << ":" << shortTypeNode2 << std::endl;
        }

        // Retrieve or cache database values for nodes
        auto getCachedValue = [](int nodeId, RocksDBHandler& db, std::unordered_map<int, std::string>& cache) -> std::string {
            auto it = cache.find(nodeId);
            if (it != cache.end()) {
                return it->second;
            } else {
                std::string value = db.getValue(std::to_string(nodeId));
                cache[nodeId] = value;
                return value;
            }
        };

        std::string fragmentOPS1 = getCachedValue(node1, opsDB, opsCache);//sg2
        std::string fragmentOPS2 = getCachedValue(node2, opsDB, opsCache);//sg3
        std::string fragmentSPO1 = getCachedValue(node1, spoDB, spoCache);//sg1
        std::string fragmentSPO2 = getCachedValue(node2, spoDB, spoCache);//sg4
        // Prepare data line
        if (isSPOFragment) {
            dataLines.emplace_back(std::to_string(node1) + " " + fragmentOPS1 + " " + std::to_string(predicate) + " " +
                                   std::to_string(node2) + " " + fragmentOPS2 + " " + fragmentSPO2 + "\n");
        } else {
            dataLines.emplace_back(std::to_string(node1) + " " + fragmentSPO1 + " " + std::to_string(predicate) + " " +
                                   std::to_string(node2) + " " + fragmentOPS2 + " " + fragmentSPO2 + "\n");
        }

        // Cache idNodeDB values and write to outputFileDic if not already written
        auto writeNodeDic = [&](int nodeId, const std::string& typeNode, const std::string& fragmentOPS, const std::string& fragmentSPO) {
            if (writtenNodes.insert(nodeId).second && !isNumericType(typeNode)) {
                std::string idNodeValue = getCachedValue(nodeId, idNodeDB, idNodeCache);
                outputFileDic << nodeId << "\t" << idNodeValue << "\t" << fragmentOPS << "\t" << fragmentSPO << "\n";
            }
        };

        writeNodeDic(node1, typeNode1, fragmentOPS1, fragmentSPO1);
        writeNodeDic(node2, typeNode2, fragmentOPS2, fragmentSPO2);

        // Process in chunks to limit memory usage
        if (dataLines.size() >= CHUNK_SIZE) {
            for (const auto& dataLine : dataLines) {
                outputFile << dataLine;
            }
            dataLines.clear();

            // Optionally clear caches if they become too large
            if (opsCache.size() > CHUNK_SIZE) opsCache.clear();
            if (spoCache.size() > CHUNK_SIZE) spoCache.clear();
            if (idNodeCache.size() > CHUNK_SIZE) idNodeCache.clear();
        }
    }

    // Write any remaining data lines to the output file
    for (const auto& dataLine : dataLines) {
        outputFile << dataLine;
    }

    // Close files
    file.close();
    outputFile.close();
    schemaFile.close();
    outputFileDic.close();
}




// Main function to reencode fragments
void FragmentReencoder::reencodeFragments() {
    std::string spoDBPath =  inputDirectory + "/node_fragment_spo";
    std::string opsDBPath = inputDirectory + "/node_fragment_ops";
    std::string idNodeDBPath = inputDirectory + "/id_node";

    // Create RocksDBHandler instances for each database
    RocksDBHandler spoDB(spoDBPath);
    RocksDBHandler opsDB(opsDBPath);
    RocksDBHandler idNodeDB(idNodeDBPath);
      // Thread-safe queue
    std::queue<std::string> fragmentQueue;
    std::mutex queueMutex;
    std::condition_variable cv;
    bool done = false;
    // Populate the queue
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        for (const auto& entry : fs::directory_iterator(inputDirectory)) {
            std::string filename = entry.path().filename().string();
            if (filename.rfind("spo_fg", 0) == 0 || filename.rfind("ops_fg", 0) == 0) {
                fragmentQueue.push(entry.path().string());
            }
        }
    }

    // Determine the number of threads to use
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 2;

    // Worker function
    auto worker = [&]() {
        while (true) {
            std::string fragmentFile;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                cv.wait(lock, [&]() { return !fragmentQueue.empty() || done; });

                if (fragmentQueue.empty()) {
                    if (done) break;
                    else continue;
                }

                fragmentFile = fragmentQueue.front();
                fragmentQueue.pop();
            }

            // Process the fragment
            encodeFragment(fragmentFile, spoDB, opsDB, idNodeDB);
        }
    };

    // Launch worker threads
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker);
    }

    // Notify workers in case there are tasks
    cv.notify_all();

    // Wait for all tasks to be processed
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        done = true;
    }
    cv.notify_all();

    // Join threads
    for (auto& t : threads) {
        t.join();
    }

}


