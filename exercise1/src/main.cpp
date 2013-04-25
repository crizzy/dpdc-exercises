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

#define COLS 223
#define BUFFER_SIZE 128000

typedef std::vector<std::string> ColumnVector;

// global vars
std::vector<ColumnVector> columns;



bool checkUniquenessFor1Column(std::vector<std::string> &column)
{
    size_t sizeBefore = column.size();
    ColumnVector::iterator it = std::unique(column.begin(), //TODO: do this for multiple columns
                                            column.end());  //TODO: end early as soon as two rows are the same -> not done by unique
    size_t sizeAfter = it - column.begin();
    
    if (sizeAfter == sizeBefore)
        return true;
    return false;
}

//TODO encode NULL with steuerzeichen
bool isUniqueColumnCombination(std::vector<int> &combination)
{//TODO make it work
    
    //print out what we are looking for
    std::cout << "looking for a UCC in the combination {";
    for (int i = 0; i< combination.size() - 1; i++)
        std::cout << combination[i] << ", ";
    std::cout << *(combination.end()-1) << "}" << std::endl;
    
    if (combination.size() > 1)
    {
        std::vector<std::string> vec_stichedColumn;
        for (int rowIndex = 0; rowIndex < columns[0].size(); rowIndex++)
        {
            std::stringstream sstr_stichedColumn(columns[combination.at(0)].at(rowIndex));
            for (int i = 1; i < combination.size(); i++)
                sstr_stichedColumn << "/t" << columns[combination.at(i)].at(rowIndex);
            vec_stichedColumn.push_back(sstr_stichedColumn.str());
        }
        return checkUniquenessFor1Column(vec_stichedColumn);
        
    }
    else
    {
        std::vector<std::string> &column = columns[combination[0]];
        return checkUniquenessFor1Column(column);
        //TODO {1, 2} sind noch false
    }
}




int readColumnsFromFile()
{
	for (int i = 0; i<COLS; i++)
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
    std::vector<int> combination = {1,2};
    
    isUniqueColumnCombination(combination) ?
    std::cout <<"true" << std::endl:
    std::cout <<"false" << std::endl;
    
    
    
    // end
    std::cout << "Finished!" << std::endl;
    char c; std::cin >> c;
}

