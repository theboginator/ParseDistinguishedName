/*
 * Given a DN provided as an argument (in quotes) this script will return the URL of the DC via console.
 * If no arguments are provided, this script will attempt to open "input_DN.txt" and read each DN in the file, output URLs written to "output_DN.txt"
 */

#include <iostream>
#include <fstream>
#include <unistd.h>
#include "parsedn.h"

int main(int argc, char * argv[]) {

    //std::cout << "My directory is: " << argv[0] << "\n";
    std::vector<struct DNentry> DNentries;
    if(argc > 1){ //If DNs were supplied via command line: read them in from argv
        int argDN = argc - 1;
        //std::cout << "Program launched with " << argDN << " arguments\n";
        for(int i = 1; i < argc; i++){ //Start at 1 because argv[0] is call to executable.
            //std::cout << "\nBuilding line: " << i << "\n";
            //std::cout << argv[i] << "\n";
            DNentries.push_back(createEntry(argv[i]));
            std::cout << DNentries[i-1].formattedURL << "\n";
        }
    } else { //Read DNs from input_DN.txt
        std::cout << "No arguments specified. Will read from 'input_DN.txt'\n";
        //Get the input file:
        std::ifstream inputFile;
        std::vector<std::string> lines;
        std::string testRow;
        inputFile.open(R"(input_DN.txt)");
        //Read the contents of the input file:
        if (inputFile.is_open()) {
            while (std::getline(inputFile, testRow)) {
                std::cout << testRow << "\n";
                lines.push_back(testRow);
            }
            inputFile.close();
        } else {
            std::cout << "Unable to open input file. Running processing on supplied arguments.\n";
        }
        //Open the output file:
        std::ofstream outputFile;
        outputFile.open(R"(output_DN.txt)");
        if(!outputFile.is_open()){
            std::cout << "Unable to open output file\n";
            exit(1);
        }

        //Take every DN and format it into the name and the URL
        for(auto & line : lines){
            std::cout << "\nBuilding line: " << line << "\n";
            struct DNentry newEntry = createEntry(line);
            outputFile << newEntry.commonName << ":" << newEntry.formattedURL << "\n";
        }
        outputFile.close();
    }


    return 0;
}
