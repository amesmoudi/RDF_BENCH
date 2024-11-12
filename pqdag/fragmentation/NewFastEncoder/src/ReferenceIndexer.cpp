#include "ReferenceIndexer.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

void ReferenceIndexer::processReferenceFile(const std::string& filePath, const std::string& type) {
    ///std::cout << "Processing reference file: " << filePath << " of type: " << type << std::endl;
    // Step 1: Sort and split the file into fragments
    sortAndSplitFile(filePath, type);

    // Step 2: Create an index for the fragments
    std::string directory = fs::path(filePath).parent_path().string();
    createIndex(directory, type);

    // Step 3: Load the index into memory
    loadIndex(directory, type);
}

void ReferenceIndexer::sortAndSplitFile(const std::string& filePath, const std::string& type) {
    std::ifstream file(filePath);
    std::string line;
    std::vector<std::pair<int, int>> references;

   // std::cout << "Reading references from file: " << filePath << std::endl;
    // Read the entire file into memory (only recommended for small to medium files)
    while (getline(file, line)) {
        std::istringstream iss(line);
        int node, fragmentID;
        if (!(iss >> node >> fragmentID)) {
            break;
        }
        references.emplace_back(node, fragmentID);
    }
    file.close();

   // std::cout << "Sorting references..." << std::endl;
    // Sort the references by node ID
    std::sort(references.begin(), references.end());

    // Split the sorted references into fragments
    std::string directory = fs::path(filePath).parent_path().string();
    std::string prefix = type + "_fragment_";
    int fragmentIndex = 0;
    std::cout << "Splitting references into fragments..." << std::endl;
    for (size_t i = 0; i < references.size(); i += MAX_FRAGMENT_SIZE) {
        std::ofstream outFile(directory + "/" + prefix + std::to_string(fragmentIndex) + ".txt");
        for (size_t j = i; j < std::min(i + MAX_FRAGMENT_SIZE, references.size()); ++j) {
            outFile << references[j].first << " " << references[j].second << "\n";
        }
        outFile.close();
        fragmentIndex++;
    }
    //std::cout << "Finished splitting references into " << fragmentIndex << " fragments." << std::endl;
}

void ReferenceIndexer::createIndex(const std::string& directory, const std::string& type) {
    std::ofstream indexFile(directory + "/" + type + "_index.txt");
    std::string prefix = type + "_fragment_";
    int fragmentIndex = 0;

    std::cout << "Creating index..." << std::endl;
    while (true) {
        std::ifstream fragmentFile(directory + "/" + prefix + std::to_string(fragmentIndex) + ".txt");
        if (!fragmentFile) {
            break;
        }

        int minNode = std::numeric_limits<int>::max();
        int maxNode = std::numeric_limits<int>::min();
        std::string line;

        while (getline(fragmentFile, line)) {
            std::istringstream iss(line);
            int node, fragmentID;
            if (!(iss >> node >> fragmentID)) {
                break;
            }
            if (node < minNode) {
                minNode = node;
            }
            if (node > maxNode) {
                maxNode = node;
            }
        }

        if (minNode != std::numeric_limits<int>::max() && maxNode != std::numeric_limits<int>::min()) {
            indexFile << minNode << " " << maxNode << " " << directory + "/" + prefix + std::to_string(fragmentIndex) + ".txt\n";
            //std::cout << "Indexed fragment " << fragmentIndex << " with range [" << minNode << ", " << maxNode << "]" << std::endl;
        }

        fragmentIndex++;
        fragmentFile.close();
    }

    indexFile.close();
    std::cout << "Index created successfully." << std::endl;
}

void ReferenceIndexer::loadIndex(const std::string& directory, const std::string& type) {
    std::ifstream indexFile(directory + "/" + type + "_index.txt");
    std::string line;

    std::map<int, std::pair<int, std::string>>& indexMap = (type == "spo") ? spoIndex : opsIndex;

    //std::cout << "Loading index from file: " << type + "_index.txt" << std::endl;
    while (getline(indexFile, line)) {
        std::istringstream iss(line);
        int minNode, maxNode;
        std::string fragmentFile;
        if (iss >> minNode >> maxNode >> fragmentFile) {
            indexMap[minNode] = { maxNode, fragmentFile };
           // std::cout << "Loaded index for range [" << minNode << ", " << maxNode << "] -> " << fragmentFile << std::endl;
        }
    }

    indexFile.close();
    //std::cout << "Index loaded successfully." << std::endl;
}

int ReferenceIndexer::getFragmentID(int node, const std::string& referenceType) {
    const std::map<int, std::pair<int, std::string>>& indexMap = (referenceType == "spo") ? spoIndex : opsIndex;
    std::unordered_map<int, int>& referenceMap = (referenceType == "spo") ? spoReferences : opsReferences;

    std::cout << "Searching for node: " << node << " in index." << std::endl;
    for (const auto& [minNode, maxNodeFilePair] : indexMap) {
        const auto& [maxNode, fragmentFile] = maxNodeFilePair;
        if (node >= minNode && node <= maxNode) {
            std::cout << "Node found in fragment range [" << minNode << ", " << maxNode << "]. Loading fragment file: " << fragmentFile << std::endl;
            loadFragmentFile(fragmentFile, referenceMap);
            break;
        }
    }

    auto refIt = referenceMap.find(node);
    if (refIt != referenceMap.end()) {
        std::cout << "Node found in fragment. Fragment ID: " << refIt->second << std::endl;
        return refIt->second;
    }

    std::cout << "Node not found in fragment." << std::endl;
    return -1;
}

void ReferenceIndexer::loadFragmentFile(const std::string& filePath, std::unordered_map<int, int>& referenceMap) {
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        std::cerr << "Error opening fragment file: " << filePath << std::endl;
        return;
    }

    std::string line;
    std::cout << "Loading data from fragment file: " << filePath << std::endl;
    while (getline(file, line)) {
        std::istringstream iss(line);
        int node, fragmentID;
        if (!(iss >> node >> fragmentID)) {
            std::cerr << "Error parsing line: " << line << std::endl;
            continue;
        }
        referenceMap[node] = fragmentID;
    }

    file.close();
    std::cout << "Finished loading data from fragment file: " << filePath << std::endl;
}
