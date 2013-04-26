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
	std::ifstream dataFile(DATA_PATH);
	if (!dataFile.is_open())
	{
		std::cout << "Cannot find file \"" << DATA_PATH << "\"" << std::endl;
		return 1;
	}
    
	std::string buffer;
	std::string elementBuffer;
	std::map<std::string, int>::iterator dictionaryEntry;
	int rowIndex = 0;

	std::cout << "Reading file \"" << DATA_PATH << "\" into RAM..." << std::endl;
    
	while (!dataFile.eof())
	{
		// Read a whole line into the buffer:   
		std::getline(dataFile, buffer, '\n');
		std::stringstream line(buffer);
        
		for (int colIndex = 0; !line.eof(); colIndex++)
		{
			if (rowIndex == 0)
				g_columns.push_back(ColumnVector());

			std::getline(line, elementBuffer, '\t');

			dictionaryEntry = dictionary.find(elementBuffer);
			if (dictionaryEntry == dictionary.end())
			{
				// element was not found in dictionary
				dictionary[elementBuffer] = distinctValues;
				g_columns.at(colIndex).push_back(distinctValues++);
			}
			else
			{
				// element already exists in dictionary
				g_columns.at(colIndex).push_back(dictionaryEntry->second);
			}
		}

        if ((++rowIndex) % 50000 == 0)
        	std::cout << "Read " << (rowIndex) << " rows. Distinct values so far: " << distinctValues << std::endl;
	}

	std::cout << "Finished reading " << (rowIndex-1) << " rows. Total distinct values: " << distinctValues << std::endl;

    return 0;
}

void printTable()
{
	size_t rowCount = g_columns.at(0).size();
	size_t colCount = g_columns.size();
	for (int row = 0; row < rowCount; row++)
	{
		for (int col = 0; col < colCount; col++)
		{
			std::cout << g_columns[col][row] << "\t";
		}
		std::cout << std::endl;
	}
}


std::vector<std::vector<std::vector<int>>> isUniqueColumnCombinationPLI(ColumnCombination &combination) // Markus //TODO rename method
{
    //if result vector is empty than it was unique
    
    std::vector<std::vector<std::vector<int>>> duplicates;
    
    
    for (int combIndex = 0; combIndex < combination.size(); combIndex++)
    {
        int columnIndex = combination[combIndex];
        
        std::map<int,std::vector<int>> dict; // temporary dictionary
        
        std::vector<int> &column = g_columns[columnIndex];
        for (int rowIndex = 0; rowIndex < column.size(); rowIndex++)
        {
            int &cell = column[rowIndex];
            if (dict.find(cell)) // key existing
                dict[column[rowIndex]].push_back(cell)
            else
                dict[column[rowIndex]] = cell;
            
            
        }
        
        // iterate over map, throw out unique values (where vector has just 1 element), and put remaining vectors in a vector
        
        
        
            
    }
        
        

    
    return duplicates; //local var?
}

ColumnCombination findNextCombination() //Crizzy
{
    
    
    
    // considers cost
    //if isCombinationExpensive()
    //    continue;
	return ColumnCombination();
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

