// sorter.cpp
#include "sorter.h"
#include <fstream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <queue>
#include <iostream>
#include <cstdlib>      
#include <stdexcept> 
#include <thread>

void Sorter::externalSort(const std::string& inputFilePath, const std::string& outputFilePath) {
    unsigned int numCores = std::thread::hardware_concurrency();
    if (numCores == 0) 
        numCores = 1; // Fallback to 1 if detection fails
    // Construct the sort command
    std::ostringstream cmd;
    cmd << "LC_ALL=C sort -n -k1,1 --parallel=" << numCores << " " << inputFilePath << " -o " << outputFilePath;
    // Execute the command
    int result = std::system(cmd.str().c_str());
    if (result != 0) 
        throw std::runtime_error("Sorting failed with exit code " + std::to_string(result));
}

void Sorter::externalSortWithPermutation(const std::string& inputFilePath, const std::string& outputFilePath) {
    unsigned int numCores = std::thread::hardware_concurrency();
    if (numCores == 0) 
        numCores = 1;
    // Construct the command
    std::ostringstream cmd;
    cmd << "awk '{print $3, $2, $1}' " << inputFilePath << " | LC_ALL=C sort -n -k1,1 --parallel=" << numCores << " -o " << outputFilePath;
    // Execute the command
    int result = std::system(cmd.str().c_str());
    if (result != 0) 
        throw std::runtime_error("Sorting by object failed with exit code " + std::to_string(result));
}




