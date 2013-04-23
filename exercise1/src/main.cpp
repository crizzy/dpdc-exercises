//
//  main.cpp
//  Exercise1
//
//  Created by Markus Hinsche on 4/23/13.
//  Copyright (c) 2013 Markus Hinsche. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <list>
#include <algorithm>    // std::unique, std::distance


#ifdef WIN32
#define DATA_PATH "../data/testfile_from_lecture.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/testfile_from_lecture.tsv"
#endif


#include <map>

#define BUFFER_SIZE 128000

typedef std::vector<std::string> ColumnVector;

// global vars
std::vector<ColumnVector> columns;


bool isUniqueColumnCombination(std::vector<int> &combination)
{//TODO make it work
    
//    for (int i = 0; i < combination.size(); i++)
//    {
//        columns[combination.at(i)];
//    }
    
    size_t sizeBefore = columns[combination.at(0)].size();
    std::vector<std::string> column = columns[combination.at(0)];
    
    ColumnVector::iterator it = std::unique(columns[combination.at(0)].begin(), //TODO: do this for multiple columns
                                         columns[combination.at(0)].end());
    size_t sizeAfter = it - columns[combination.at(0)].begin();
    
    if (sizeAfter == sizeBefore)
        return true;
    
    return false;
}




int readColumnsFromFile()
{
	for (int i = 0; i<223; i++)
	{
		columns.push_back(ColumnVector());
	}
    
	// read column names:
	std::ifstream I(DATA_PATH);
	if (!I.is_open())
	{
		std::cout << "Cannot find file \"uniprot.tsv\"" << std::endl;
		return 1;
	}
    
	char rowBuffer[BUFFER_SIZE];
	char cellBuffer[BUFFER_SIZE];
    
	for (int rowIndex = 0; !I.eof(); rowIndex++)
	{
		// read a whole line into the buffer:
		I.getline(rowBuffer, BUFFER_SIZE, '\n');
		if (strlen(rowBuffer) == 0)
			break;
        
		std::stringstream s_rowBuffer(rowBuffer);
        
		for (int colIndex = 0; !s_rowBuffer.eof(); colIndex++)
		{
			s_rowBuffer.getline(cellBuffer, BUFFER_SIZE, '\t');
			columns.at(colIndex).push_back(cellBuffer);
			std::cout << cellBuffer << "\t";
		}
		std::cout << std::endl;
//		if (rowIndex % 54000 == 0)
//			std::cout << "read " << rowIndex << " rows." << std::endl;
	}
    
		
    
    return 0;
}

int main(int argc, const char * argv[])
{
    // read stuff
    readColumnsFromFile();
    
    // check if read correctly
//    for(ColumnVector::iterator it = columns[0].begin(); it != columns[0].end();it++)
//    {
//        std::cout << *it<<std::endl;
//    }
    
    
    // unique column combinations
    std::vector<int> combination = {3};
    
    isUniqueColumnCombination(combination) ?
    std::cout <<"true" << std::endl:
    std::cout <<"false" << std::endl;
    
    
    
    // end
    std::cout << "Finished!" << std::endl;
    char c; std::cin >> c;
}

