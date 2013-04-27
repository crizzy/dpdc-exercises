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
#include <set>
#include <algorithm>    // std::unique, std::distance std::intersection std::sort
#include <map>
#include <time.h>
#include <inttypes.h>

#ifdef _WIN32
#define DATA_PATH "../data/uniprot.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/uniprot.tsv"
#endif

typedef std::vector<std::set<int>> ColumnVector;
typedef std::vector<short> ColumnCombination;
typedef std::map<std::string, int> Dictionary;

// global vars
std::vector<ColumnVector> g_columns;
std::map<int, ColumnCombination> g_costs;
int64_t g_columnCount = 0;
int g_rowCount = 1000000000;//important: must be initialized to artificially high value

const float searchDensity = 0.1f;

float g_timeForIntersection;
float g_timeTotal;


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
	std::ifstream dataFileOnDisc(DATA_PATH);
	if (!dataFileOnDisc.is_open())
	{
		std::cout << "Cannot find file \"" << DATA_PATH << "\"" << std::endl;
		return 1;
	}
    
	std::string tableValue;
	Dictionary::iterator dictionaryEntry;
	std::vector<std::streamoff> columnReadPositions;

	std::cout << "Reading file \"" << DATA_PATH << "\" into RAM...";

	std::stringstream dataFile;
	dataFile << dataFileOnDisc.rdbuf();
	dataFileOnDisc.close();

	std::cout << "done!" << std::endl;

	// first, determine the number of columns:
	std::getline(dataFile, tableValue, '\n');
	g_columnCount = std::count(tableValue.begin(), tableValue.end(), '\t') + 1;
	std::cout << "Detected " << g_columnCount << " columns." << std::endl;
    
	for (int colIndex = 0; colIndex < g_columnCount; colIndex++)
	{
		dataFile.clear();//clear eof bit
		dataFile.seekg(0);

		Dictionary dictionary;//the dictionary holding distinct values of this column
		int dictionarySize = 0;//size of the dictionary for fast access
		ColumnVector currentColumnVector;//our column vector to be built
		ColumnVector cleanColumnVector;

		int rowIndex = 0;
		for (rowIndex = 0; !dataFile.eof() && rowIndex < g_rowCount; rowIndex++)
		{
			if (colIndex > 0)
			{
				dataFile.seekg(columnReadPositions[rowIndex]);
			}

			//read next table value in the current column:
			std::getline(dataFile, tableValue, (colIndex < g_columnCount-1) ? '\t' : '\n');

			if (dataFile.eof())
				break;

			//save the current read position for later jumping:
			if (colIndex == 0)
			{
				columnReadPositions.push_back(dataFile.tellg());
			}
			else
			{
				columnReadPositions[rowIndex] = dataFile.tellg();
			}

			if (colIndex < g_columnCount-1)
			{
				//go to the end of the line:
				dataFile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}

			dictionaryEntry = dictionary.find(tableValue);
			if (dictionaryEntry == dictionary.end())
			{
				// value has not been found in this column
				dictionary[tableValue] = dictionarySize++;
				std::set<int> s;
				s.insert(rowIndex);
				currentColumnVector.push_back(s);
			}
			else
			{
				// value is already part of the column
				currentColumnVector[dictionaryEntry->second].insert(rowIndex);
			}
		}

		//Create a new "clean" column vector based on the current one, ignoring all sets with cardinality 1:
		int n = 0;
		for (std::vector<std::set<int>>::iterator it = currentColumnVector.begin(); it != currentColumnVector.end(); it++)
		{
			if (it->size() > 1)
				cleanColumnVector.push_back(*it);
		}

		if (colIndex == 0)
			std::cout << "Detected " << (g_rowCount = rowIndex) << " rows." << std::endl;

		// determine number of unique values in this column:
		int uniquesCount = g_rowCount;
		for (int setId = 0; setId < cleanColumnVector.size(); setId++)
		{
			uniquesCount -= cleanColumnVector[setId].size();
		}

		std::cout << "Column " << colIndex << ": " << cleanColumnVector.size() << (cleanColumnVector.size() == 1 ? " set" : " sets") << " and " << uniquesCount << " uniques.";

		if (uniquesCount == g_rowCount)
		{
			// drop column, if only containing unique values:
			std::cout << " => Dropped, because column is already unique.";
		}
		else if (uniquesCount < g_rowCount * searchDensity)
		{
			// drop column, if there are too few unique values:
			std::cout << " => Dropped because of too many duplicates.";
		}
		else
		{
			// take column into consideration for later combinations
			g_columns.push_back(cleanColumnVector);
		}

		std::cout << std::endl;
	}

	std::cout << "Finished reading " << g_columnCount << " columns." << std::endl;

    return 0;
}

void printTable()
{
	size_t colCount = g_columns.size();
	for (int col = 0; col < colCount; col++)
	{
		std::cout << "Column " << col << ": ";

		//Determine size of column:
		int columnSize = 0;
		for (int duplicate = 0; duplicate < g_columns[col].size(); duplicate++)
		{
			columnSize += g_columns[col][duplicate].size();
		}

		if (columnSize < 20)
		{
			//Column is small enough to be printed in detail:
			std::cout << "{";
			for (int duplicate = 0; duplicate < g_columns[col].size(); duplicate++)
			{
				std::set<int>::iterator c = g_columns[col][duplicate].begin();
				if (c != g_columns[col][duplicate].end())
					std::cout << "{" << (*c);
				while (++c != g_columns[col][duplicate].end())
				{
					std::cout << ", " << (*c);
				}
				std::cout << "}";
			}
			std::cout << "}\n";
		}
		else
		{
			//Column is big, so only print the number of sets of duplicates:
			std::cout << g_columns[col].size() << " sets of duplicates\n";
		}
	}
}


/*std::vector<std::vector<int>> isUniqueColumnCombinationPLI(ColumnCombination &combination) // Markus //TODO rename method // for comb with size 2
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
    for (it1 = vec_of_columnDuplicates[0].begin(); it1 < vec_of_columnDuplicates[0].end(); ++it1)
    {
        for (it2 = vec_of_columnDuplicates[1].begin(); it2 < vec_of_columnDuplicates[1].end(); ++it2)
        {
            time_t beginTimeIntersection = clock();
            
            std::vector<int> v(it1->size() + it2->size()); //possible without making new vector
            // we assume that (*it1) is sorted
            std::vector<int>::iterator it = std::set_intersection(it1->begin(), it1->end(), it2->begin(), it2->end(), v.begin());// takes a lot of time
            v.resize(it - v.begin());
            
            if (v.size() > 1)
            {
                duplicates.push_back(v);
                //return duplicates; // this makes intersection time shrink a lot, but results can't be propagated
            }
            
            time_t endTimeIntersection = clock();
            g_timeForIntersection += (float(endTimeIntersection - beginTimeIntersection)) / CLOCKS_PER_SEC;
        }
    }
    return duplicates; 
}*/

ColumnCombination findNextCombination() //Crizzy
{
 
    // considers cost
    //if isCombinationExpensive()
    //    continue;
	return ColumnCombination();
}

int main(int argc, const char * argv[])
{
    g_timeForIntersection = 0.;
    time_t beginTimeTotal = clock();
    
    // read stuff
    readColumnsFromFile();
	printTable();
    
    // check if read correctly
//    for(ColumnVector::iterator it = g_columns[0].begin(); it != g_columns[0].end();it++)
//    {
//        std::cout << *it << std::endl;
//    }

    
    // Erkenntnis: bei den 1er Kombinationen sind spalte 0 und 1 unique
    
    /*for (int i = 2; i < g_columns.size(); i++) // TODO: use sorted list instead, so that cheap combinations are done first
    {
        for (int j = i+1; j < g_columns.size(); j++)
        {
            ColumnCombination combination;
            combination.push_back(i);
            combination.push_back(j);
            // if i and j are the same that is like just checking 1 column
            
            std::vector<std::vector<int>> remainingRows = isUniqueColumnCombinationPLI(combination);
            if (remainingRows.empty()) // unique column combination
                std::cout << "found unique column combination: {" << combination[0] << ", " << combination[1] << "}" << std::endl;
            
            std::vector<std::vector<int>> g_costs = isUniqueColumnCombinationPLI(combination); // TODO give return value as parameter
            
            //remember the return values= g_costs
            
            time_t middleTimeTotal = clock();
            g_timeTotal = float((middleTimeTotal - beginTimeTotal)) / CLOCKS_PER_SEC;
            
            //std::cout << "timeTotal: " << g_timeTotal << ", timeIntersection: " << g_timeForIntersection << std::endl;
        }
    }*/
    
    
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

//TODO auf kleinem datensatz

//Nico: anfang alles in hashmap rein, diese mappt dann key auf ein array

//8min zum einlesen auf mac:( // 1min auf Release-Scheme :)

