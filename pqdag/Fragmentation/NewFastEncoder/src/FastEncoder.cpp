//============================================================================
// Name        : RDFDataEncoder
// Version     :
// Copyright   : KAUST-Infocloud
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "partitioner_store.h"
#include "fragmenter.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include "sorter.h"
#include "ReferenceIndexer.h"
#include "FragmentReencoder.h"
#include "DictionaryIndexer.h"
#include <iostream>

#include <fstream>
#include <string>
#include <sys/stat.h> 
int main(int argc, char** argv) {
	if(argc < 3){
		print_to_screen("USAGE: RDFEncoder input_dir output_file_path");
		exit(-1);
	}
	Profiler profiler;
	profiler.startTimer("all");
	string input_dir = string(argv[1]);
	string output_file_name = "/bindata/data.nt";
	/*********************** Encode The RDF Data ******************/
	partitioner_store * store;
    store = new partitioner_store();
	store->load_encode_rdf_data(input_dir, output_file_name);
	delete store;
	/****************************************************************/
	
	/************************* Sort The Encoded RDF Data Based On The Subject And On The Object **************************/
	print_to_screen(part_string);
	print_to_screen("Sorting Triples!");
	profiler.startTimer("sorting_SPO_OPS");
    Sorter sorter;  // Création d'une instance de Sorter
    sorter.externalSort(output_file_name, output_file_name + "_spo");  
    sorter.externalSortWithPermutation(output_file_name, output_file_name + "_ops");  
	profiler.pauseTimer("sorting_SPO_OPS");
	print_to_screen("Done with Sorting in " + toString(profiler.readPeriod("sorting_SPO_OPS")) + " sec");
	profiler.clearTimer("sorting_SPO_OPS");
	/**********************************************************************************************************************/
	
	/********************************************** Fragmentation *********************************************************/
	print_to_screen(part_string);
	print_to_screen("Fragmentation!");
	profiler.startTimer("fragmentation");
    Fragmenter fragmenter;
    fragmenter.processFiles(output_file_name + "_spo",output_file_name + "_ops");
	profiler.pauseTimer("fragmentation");
	print_to_screen("Done with Fragmentation in " + toString(profiler.readPeriod("fragmentation")) + " sec");
	profiler.clearTimer("fragmentation");
	print_to_screen(part_string);
	/**********************************************************************************************************************/

	/********************************************** Reversing the dictionary file *****************************************/
	print_to_screen(part_string);
    print_to_screen("Indexating Dico!");
	DictionaryIndexer dicoIndexer;
	dicoIndexer.reverseDictionaryFile("/bindata/nodes.dic");
	/**********************************************************************************************************************/
	
	/********************************************** Fragments Reencoding *****************************************/
	print_to_screen(part_string);
	print_to_screen("Fragments re-encoding!");
	profiler.startTimer("reencoding");
	std::string inputDirectory = "/bindata";
    std::string outputDirectory = "/outputdata";
    FragmentReencoder reencoder(inputDirectory, outputDirectory);
    reencoder.reencodeFragments();
	profiler.pauseTimer("reencoding");
	print_to_screen("Done with Fragments re-encoding in " + toString(profiler.readPeriod("reencoding")) + " sec");
	profiler.clearTimer("reencoding");
	print_to_screen(part_string);
	profiler.pauseTimer("all");
	print_to_screen("Run Finished in " + toString(profiler.readPeriod("all")) + " sec");
	profiler.clearTimer("all");
	/*******************************************************************************************************************/

	/************************************ Cpy Schema.txt , spo_index.txt, and ops_index.txt *****************************/
	    // Copy /bindata/schema.txt to /outputdata/predicates.txt and replace all "String" with "string"
    {
        std::ifstream inputFile("/bindata/schema.txt");
        if (!inputFile.is_open()) {
            std::cerr << "Error opening /bindata/schema.txt for reading." << std::endl;
            return -1;
        }
        std::ofstream outputFile("/outputdata/predicates.txt");
        if (!outputFile.is_open()) {
            std::cerr << "Error opening /outputdata/predicates.txt for writing." << std::endl;
            return -1;
        }

        std::string line;
        while (std::getline(inputFile, line)) {
            // Replace all occurrences of "String" with "string"
            size_t pos = 0;
            while ((pos = line.find("String", pos)) != std::string::npos) {
                line.replace(pos, 6, "string");
                pos += 6;
            }
            outputFile << line << '\n';
        }

        inputFile.close();
        outputFile.close();
    }

    // Copy /bindata/spo_index.txt and /bindata/ops_index.txt to /outputdata
    {
        // Ensure /outputdata directory exists
        struct stat info;
        if (stat("/outputdata", &info) != 0 || !(info.st_mode & S_IFDIR)) {
            std::cerr << "Output directory does not exist: /outputdata" << std::endl;
            return -1;
        }

        // Copy /bindata/spo_index.txt to /outputdata/spo_index.txt
        std::ifstream src1("/bindata/spo_index.txt", std::ios::binary);
        std::ofstream dst1("/outputdata/spo_index.txt", std::ios::binary);
        if (!src1.is_open() || !dst1.is_open()) {
            std::cerr << "Error copying /bindata/spo_index.txt to /outputdata/spo_index.txt" << std::endl;
            return -1;
        }
        dst1 << src1.rdbuf();
        src1.close();
        dst1.close();

        // Copy /bindata/ops_index.txt to /outputdata/ops_index.txt
        std::ifstream src2("/bindata/ops_index.txt", std::ios::binary);
        std::ofstream dst2("/outputdata/ops_index.txt", std::ios::binary);
        if (!src2.is_open() || !dst2.is_open()) {
            std::cerr << "Error copying /bindata/ops_index.txt to /outputdata/ops_index.txt" << std::endl;
            return -1;
        }
        dst2 << src2.rdbuf();
        src2.close();
        dst2.close();
    }

	return 0;
}
