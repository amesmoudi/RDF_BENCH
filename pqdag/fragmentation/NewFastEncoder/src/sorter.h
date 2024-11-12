// sorter.h
#ifndef SORTER_H
#define SORTER_H

#include <string>

class Sorter {
public:
    void externalSort(const std::string& inputFilePath, const std::string& outputFilePath);
    void externalSortWithPermutation(const std::string& inputFilePath, const std::string& outputFilePath);
};

#endif // SORTER_H
