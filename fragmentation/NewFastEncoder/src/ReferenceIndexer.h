#ifndef REFERENCE_INDEXER_H
#define REFERENCE_INDEXER_H

#include <string>
#include <unordered_map>
#include <map>

class ReferenceIndexer {
public:
    void processReferenceFile(const std::string& filePath, const std::string& type);
    int getFragmentID(int node, const std::string& referenceType);

private:
    void sortAndSplitFile(const std::string& filePath, const std::string& type);
    void createIndex(const std::string& directory, const std::string& type);
    void loadIndex(const std::string& directory, const std::string& type);
    void loadFragmentFile(const std::string& filePath, std::unordered_map<int, int>& referenceMap);

    std::unordered_map<int, int> spoReferences;
    std::unordered_map<int, int> opsReferences;
    std::map<int, std::pair<int, std::string>> spoIndex; // min_node, max_node, fragment_file
    std::map<int, std::pair<int, std::string>> opsIndex; // min_node, max_node, fragment_file

    const size_t MAX_FRAGMENT_SIZE = 100000;
};

#endif // REFERENCE_INDEXER_H
