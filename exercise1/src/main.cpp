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
#include <map>



#ifdef _WIN32
#define DATA_PATH "../data/uniprot.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/testfile_from_lecture.tsv"
#endif


#define COLS 223
#define BUFFER_SIZE 128000


typedef std::vector<int> ColumnVector;
typedef std::vector<int> ColumnCombination;

// global vars
std::vector<ColumnVector> g_columns;
std::map<int, ColumnCombination> g_costs;




/*bool checkUniquenessFor1Column(std::vector<int> &column)
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
        for (int rowIndex = 0; rowIndex < g_columns[0].size(); rowIndex++)
        {
            std::stringstream sstr_stichedColumn(g_columns[combination.at(0)].at(rowIndex));
            for (int i = 1; i < combination.size(); i++)
                sstr_stichedColumn << "/t" << g_columns[combination.at(i)].at(rowIndex);
            vec_stichedColumn.push_back(sstr_stichedColumn.str());
        }
        return checkUniquenessFor1Column(vec_stichedColumn);
        
    }
    else
    {
        std::vector<std::string> &column = g_columns[combination[0]];
        return checkUniquenessFor1Column(column);
        //TODO {1, 2} sind noch false
    }
}*/




int readColumnsFromFile()
{
	std::map<std::string, int> dictionary;
	unsigned int distinctValues = 0;
    
	// read column names:
	std::ifstream I(DATA_PATH);
	if (!I.is_open())
	{
		std::cout << "Cannot find file \"" << DATA_PATH << "\"" << std::endl;
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
			if (rowIndex == 0)
				g_columns.push_back(ColumnVector());

			s_rowBuffer.getline(cellBuffer, BUFFER_SIZE, '\t');

			std::map<std::string, int>::iterator dictionaryEntry = dictionary.find(cellBuffer);
			if (dictionaryEntry == dictionary.end())
			{
				// element was not found in dictionary
				dictionary[cellBuffer] = distinctValues;
				g_columns.at(colIndex).push_back(distinctValues++);
			}
			else
			{
				// element already exists in dictionary
				g_columns.at(colIndex).push_back(dictionary[cellBuffer]);
			}

			//std::cout << cellBuffer << "\t";
		}
		//std::cout << std::endl;
        //		if (rowIndex % 54000 == 0)
        //			std::cout << "read " << rowIndex << " rows." << std::endl;
	}
    
    
    
    return 0;
}

void printTable()
{
	int rowCount = g_columns.at(0).size();
	int colCount = g_columns.size();
	for (int row = 0; row < rowCount; row++)
	{
		for (int col = 0; col < colCount; col++)
		{
			std::cout << g_columns[col][row] << "\t";
		}
		std::cout << std::endl;
	}
}


std::vector<std::vector<int>> isUniqueColumnCombinationPLI(ColumnCombination &combination) // Markus
{
    //if result vector is empty than it was unique
    return vector;
}

ColumnCombination findNextCombination() //Crizzy
{
    
    
    
    // considers cost
    if isCombinationExpensive()
        continue;
}

int main(int argc, const char * argv[])
{
    // read stuff
    readColumnsFromFile();
    
    // check if read correctly
    //    for(ColumnVector::iterator it = g_columns[0].begin(); it != g_columns[0].end();it++)
    //    {
    //        std::cout << *it<<std::endl;
    //    }

        
    for (int i = 0; i < g_columns.size(); i++)
    {
        ColumnCombination combination;
        combination.push_back(i);
        std::vector<std::vector<int>> &g_costs = isUniqueColumnCombinationPLI(combination);
        
        //remember the return values= g_costs
        
    }
    
    
    while (true)
    {
        ColumnCombination findNextCombination();
        
       
    }
    
    // unique column combinations
    std::vector<int> combination;
	combination.push_back(1);
	combination.push_back(2);
    
    /*isUniqueColumnCombination(combination) ?
    std::cout <<"true" << std::endl:
    std::cout <<"false" << std::endl;*/
    
    //printTable();
    
    // end
    std::cout << "Finished!" << std::endl;
    char c; std::cin >> c;
}

