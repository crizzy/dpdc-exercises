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
#include <algorithm>    // std::unique, std::distance std::intersection std::sort
#include <map>



#ifdef _WIN32
#define DATA_PATH "../data/uniprot.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/uniprot_20rows.tsv"
#endif



typedef std::vector<int> ColumnVector;
typedef std::vector<short> ColumnCombination;

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


std::vector<std::vector<int>> isUniqueColumnCombinationPLI(ColumnCombination &combination) // Markus //TODO rename method // for comb with size 2
{
    //if result vector is empty than it was unique

    if (combination.size() != 2)
    {
        std::cout << "isUniqueColumnCombinationPLI() assumes combination of size 2" << std::endl;
        return std::vector<std::vector<int>>();
    }
    std::vector<std::vector<int>> duplicates = std::vector<std::vector<int>>(); // AB = {{r1,r3},{},{}}
    
    std::vector<std::vector<std::vector<int>>> vec_of_columnDuplicates; // A: {{r1, r2, r3}, {r4, r5}}; B: ...
    vec_of_columnDuplicates.push_back(std::vector<std::vector<int>>()); // A: {{r1, r2, r3}, {r4, r5}} für ersten Durchlauf
    vec_of_columnDuplicates.push_back(std::vector<std::vector<int>>());
    
    for (int combIndex = 0; combIndex < combination.size(); combIndex++)
    {
        // this preprocesses 1 column (1. PLI slide)
        
        int columnIndex = combination[combIndex];
        std::map<int,std::vector<int>> dict; // A: a->{r1, r2, r3}, b->{r4, r5}
        
        std::vector<int> &column = g_columns[columnIndex];
        for (int rowIndex = 0; rowIndex < column.size(); rowIndex++)
        {
            int &cell = column[rowIndex];
            std::map<int,std::vector<int>>::iterator it = dict.find(cell);
            if (it == dict.end()) // key not existing yet
            {
                // build new vector with current row in it
                std::vector<int> vec = std::vector<int>();
                vec.push_back(rowIndex);
                dict[cell] = vec;
            }
            else // key existing
            {
                it->second.push_back(rowIndex);
            } 
        }
        
        // iterate over map, throw out unique values (where vector has just 1 element), and put remaining vectors in a vector
        std::map<int,std::vector<int>>::iterator it;
        for (it=dict.begin(); it!=dict.end(); it++)
        {
            if (it->second.size() > 1)
                vec_of_columnDuplicates[combIndex].push_back(it->second); //assume it->second is sorted
        }
        
    }
    
    //pairwise intersection
    std::vector<std::vector<int>>::iterator it1; // iterator for column A
    std::vector<std::vector<int>>::iterator it2; // iterator for column B
    for (it1 = vec_of_columnDuplicates[0].begin(); it1 < vec_of_columnDuplicates[0].end(); it1++)
    {
        for (it2 = vec_of_columnDuplicates[1].begin(); it2 < vec_of_columnDuplicates[1].end(); it2++)
        {
            std::vector<int> v(it1->size() + it2->size());
            // we assume that (*it1) is sorted
            std::vector<int>::iterator it = std::set_intersection(it1->begin(), it1->end(), it2->begin(), it2->end(), v.begin());
            v.resize(it - v.begin());
            
            if (v.size() > 1)
                duplicates.push_back(v);
        }
    }
    return duplicates; 
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
//        std::cout << *it << std::endl;
//    }

    
    for (int i = 0; i < g_columns.size(); i++)
    {
        //for (int j = i+1; j < g_columns.size(); j++)
        {
            ColumnCombination combination;
            combination.push_back(i);
            combination.push_back(i);
            // if i and j are the same that is like just checking 1 column
            
            std::vector<std::vector<int>> remainingRows = isUniqueColumnCombinationPLI(combination);
            if (remainingRows.empty()) // unique column combination
                std::cout << "found unique column combination: {" << combination[0] << ", " << combination[1] << "}" << std::endl;
            
            std::vector<std::vector<int>> g_costs = isUniqueColumnCombinationPLI(combination); // TODO give return value as parameter
            
            //remember the return values= g_costs
        }
    }
    
    
//    while (true)
//    {
//        ColumnCombination findNextCombination();
//        
//       
//    }

    
    // end
    std::cout << "Finished!" << std::endl;
    char c; std::cin >> c;
}

