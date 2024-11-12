#ifndef FRAGMENT_REENCODER_H
#define FRAGMENT_REENCODER_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <filesystem>
#include "RocksDBHandler.h"
namespace fs = std::filesystem;

class FragmentReencoder {
public:
    FragmentReencoder(const std::string& inputDir, const std::string& outputDir);
    void reencodeFragments();

private:
    std::string inputDirectory;
    std::string outputDirectory;
    std::unordered_map<int, std::tuple<std::string, std::string, std::string>> schemaMap;
    void loadSchema(const std::string& schemaFile);
    void encodeFragment(const std::string& fragmentFile,RocksDBHandler& spoDB,RocksDBHandler& opsDB,RocksDBHandler& idNodeDB);
    bool isNumericType(const std::string& type);
    std::string convertTypeToShortForm(const std::string& type);
};
#endif // FRAGMENT_REENCODER_H
