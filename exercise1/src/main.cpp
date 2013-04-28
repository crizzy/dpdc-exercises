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
#include <iterator>		// std::inserter
#include <algorithm>    // std::unique, std::distance std::intersection std::sort
#include <map>
#include <time.h>

#ifdef _WIN32
#define DATA_PATH "../data/uniprot_20rows.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/testfile_from_lecture.tsv"
#include <inttypes.h>
#endif

//#define VERBOSE
const float searchDensity = 0; // the bigger the more we throw out

typedef std::map<std::string, int> Dictionary;

class ColumnCombination : public std::vector<int>
{
	// A column combination is a set of column indices.

public:

	ColumnCombination()
	{
		//creates an empty column combination
	}

	ColumnCombination(int index)
	{
		//creates a column combination containing only one index
		push_back(index);
	}

	ColumnCombination(int firstIndex, int secondIndex)
	{
		//creates a column combination containing two indices
		push_back(firstIndex);
		push_back(secondIndex);
	}

	std::string toString()
	{
		std::stringstream columnCombinationString;
		ColumnCombination::iterator c = begin();
		columnCombinationString << "{";
		if (c != end())
		{
			columnCombinationString << (*c);
			while (++c != end())
			{
				columnCombinationString << ", " << (*c);
			}
		}
		columnCombinationString << "}";
		return columnCombinationString.str();
	}

	std::string toTabbedString()
	{
		std::stringstream columnCombinationString;
		ColumnCombination::iterator c = begin();
		if (c != end())
		{
			columnCombinationString << (*c);
			while (++c != end())
			{
				columnCombinationString << '\t' << (*c);
			}
		}
		return columnCombinationString.str();
	}

	int maxIndex()
	{
		int max = 0;
		for (ColumnCombination::iterator c = begin(); c != end(); c++)
		{
			if (max < *c)
				max = *c;
		}
		return max;
	}
};

class ColumnVector : public std::vector<std::vector<int>>
{
	// A column vector is a vector of sets, containing the indices of duplicates

public:

	ColumnCombination combination;
	int uniques;
};

// global vars
std::vector<ColumnVector> g_columns;//all single columns
std::vector<ColumnVector> g_combinedColumns[2];//column combinations
std::vector<ColumnCombination> g_results;
std::ofstream g_outputFile;

long long g_columnCount = 0;

std::map<int, ColumnCombination> g_costs;
int g_rowCount = 1000000000;//important: must be initialized to artificially high value

float g_timeForIntersection;
float g_timeTotal;


int getNumberOfUniques(ColumnVector &cv)
{
	int uniques = g_rowCount;
	for (ColumnVector::iterator set = cv.begin(); set != cv.end(); ++set)
	{
		uniques -= (int)set->size();
	}
	return uniques;
}

void reportUniqueColumnCombination(ColumnCombination &c)
{
	#ifdef VERBOSE
		std::cout << " => Dropped, because column combination is unique: " << c.toString();
	#endif
	g_results.push_back(c);
	g_outputFile << c.toTabbedString() << std::endl;
}

std::string durationToString(time_t duration)
{
	std::stringstream ss;
	if (duration >= 24*60*60*1000)
		ss << (duration / (24*60*60*1000)) << ":";
	if (duration >= 60*60*1000)
		ss << ((duration % (24*60*60*1000)) / (60*60*1000)) << ":";
	if (duration >= 60*1000)
		ss << ((duration % (60*60*1000)) / (60*1000)) << ":";

	ss << ((duration % (60*1000)) / 1000.0f);
	return ss.str();
}

int readColumnsFromFile()
{
	time_t readStartTime = clock();
	std::ifstream dataFileOnDisc(DATA_PATH);
	if (!dataFileOnDisc.is_open())
	{
		std::cout << "Cannot find file \"" << DATA_PATH << "\"" << std::endl;
		return 1;
	}

	//Create a results file and overwrite the existing one, if present:
	g_outputFile.open(std::string(DATA_PATH) + std::string("_uniques.txt"), std::ios::out | std::ios::trunc);
	if (!g_outputFile.is_open())
	{
		std::cout << "Cannot write to output file \"" << std::string(DATA_PATH) << "_uniques.txt\"" << std::endl;
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
				std::vector<int> s;
				s.push_back(rowIndex);
				currentColumnVector.push_back(s);
			}
			else
			{
				// value is already part of the column
				currentColumnVector[dictionaryEntry->second].push_back(rowIndex);
			}
		}

		//Create a new "clean" column vector based on the current one, ignoring all sets with cardinality 1:
		int n = 0;
		for (ColumnVector::iterator it = currentColumnVector.begin(); it != currentColumnVector.end(); it++)
		{
			if (it->size() > 1)
				cleanColumnVector.push_back(*it);
		}

		if (colIndex == 0)
			std::cout << "Detected " << (g_rowCount = rowIndex) << " rows." << std::endl;

		// determine number of unique values in this column:
		int uniquesCount = getNumberOfUniques(cleanColumnVector);

		std::cout << "Column " << colIndex << ": " << cleanColumnVector.size() << (cleanColumnVector.size() == 1 ? " set" : " sets") << " and " << uniquesCount << " uniques.";

		if (uniquesCount == g_rowCount)
		{
			// drop column, if only containing unique values:
			reportUniqueColumnCombination(ColumnCombination(colIndex));
		}
		else if (uniquesCount < g_rowCount * searchDensity)
		{
			// drop column, if there are too few unique values:
			std::cout << " => Dropped because of too many duplicates.";
		}
		else
		{
			// take column into consideration for later combinations
			cleanColumnVector.combination.push_back(colIndex);
			cleanColumnVector.uniques = uniquesCount;
			g_columns.push_back(cleanColumnVector);
		}

		std::cout << std::endl;
	}

	std::cout << "Finished reading " << g_columnCount << " columns in " << durationToString(clock() - readStartTime) << "." << std::endl;

	g_columnCount = g_columns.size();

    return 0;
}

void printTable()
{
	size_t colCount = g_columns.size();
	for (int col = 0; col < colCount; col++)
	{
		std::cout << "Column " << g_columns[col].combination.toString() << ": ";

		int uniques = getNumberOfUniques(g_columns[col]);

		if ((g_rowCount - uniques) < 20)
		{
			//Sets are small enough to be printed in detail:
			std::cout << "{";
			for (int duplicate = 0; duplicate < g_columns[col].size(); duplicate++)
			{
				std::vector<int>::iterator c = g_columns[col][duplicate].begin();
				if (c != g_columns[col][duplicate].end())
					std::cout << "{" << (*c);
				while (++c != g_columns[col][duplicate].end())
				{
					std::cout << ", " << (*c);
				}
				std::cout << "}";
			}
			std::cout << "}";
		}
		else
		{
			//Sets are big, so only print the number of sets of duplicates:
			std::cout << g_columns[col].size() << (g_columns[col].size() == 1 ? " set" : " sets");
		}

		std::cout << ", " << uniques << " uniques\n";
	}
}

int main(int argc, const char * argv[])
{
    time_t beginTimeTotal = clock();
    
    // read stuff
    readColumnsFromFile();
	//printTable();

	std::cout << "Operating on a subset of " << g_columnCount << " columns." << std::endl;
    
	int columnCount = (int)g_columns.size();

	std::vector<int> intersection;
	std::vector<ColumnVector> *sourceTable = &g_columns, *targetTable = &g_combinedColumns[1];
	int uniques;
	time_t timeforOneDimension;
	ColumnVector *rightColumnVector;

	for (int columnDimension = 2; columnDimension < g_columnCount; columnDimension++)
	{
		std::cout << "\nBuilding " << columnDimension << "-dimensional column combinations... (" << sourceTable->size() << "x" << g_columns.size() << " columns = " << (sourceTable->size() * g_columns.size()) << " intersections)";
		timeforOneDimension = clock();
		for (std::vector<ColumnVector>::iterator leftColumnVector = sourceTable->begin(); leftColumnVector != sourceTable->end(); leftColumnVector++)
		{
			for (int rightColumnIndex = leftColumnVector->combination.maxIndex(); rightColumnIndex < columnCount; ++rightColumnIndex)
			{
				ColumnVector intersectedColumnVector;
				rightColumnVector = &g_columns[rightColumnIndex];

				#ifdef VERBOSE
					std::cout << "\nIntersecting " << leftColumnVector->combination.toString() << " with " << rightColumnVector->combination.toString() << "...";
				#endif

				for (ColumnVector::iterator leftSet = leftColumnVector->begin(); leftSet != leftColumnVector->end(); ++leftSet)
				{
					for (ColumnVector::iterator rightSet = rightColumnVector->begin(); rightSet != rightColumnVector->end(); ++rightSet)
					{
						std::set_intersection(
							leftSet->begin(),
							leftSet->end(),
							rightSet->begin(),
							rightSet->end(),
							std::inserter(intersection, intersection.begin())
						);
						if (intersection.size() > 1)
							intersectedColumnVector.push_back(intersection);
						intersection.clear();
					}
				}

				uniques = getNumberOfUniques(intersectedColumnVector);

				#ifdef VERBOSE
					std::cout << " " << intersectedColumnVector.size() << (intersectedColumnVector.size() == 1 ? " set" : " sets") << " and " << uniques << " uniques.";
				#endif

				// only add the resulting column vector to our search set if it includes some improvement:
				if (uniques != std::max<int>(leftColumnVector->uniques, rightColumnVector->uniques))
				{
					std::set_union(
						leftColumnVector->combination.begin(),
						leftColumnVector->combination.end(),
						rightColumnVector->combination.begin(),
						rightColumnVector->combination.end(),
						std::back_inserter(intersectedColumnVector.combination)
					);

					// don't add the column vector if the created combination is a unique one
					if (intersectedColumnVector.size() == 0)
					{
						reportUniqueColumnCombination(intersectedColumnVector.combination);
						continue;
					}

					intersectedColumnVector.uniques = uniques;
					targetTable->push_back(intersectedColumnVector);
				}
			}
		}
		std::cout << "\nDuration: " << durationToString(clock() - timeforOneDimension) << std::endl;

		// clear source table:
		if (columnDimension > 2)
			sourceTable->clear();

		// invert roles of source and target table:
		sourceTable = &g_combinedColumns[1 - (columnDimension % 2)];
		targetTable = &g_combinedColumns[columnDimension % 2];
	}

    std::cout << "\n\nDone in " << durationToString(clock() - beginTimeTotal) << ". Found " << g_results.size() << " unique column combination" << (g_results.size() != 1 ? "s" : "") << ": " << std::endl;
	for (std::vector<ColumnCombination>::iterator c = g_results.begin(); c != g_results.end(); c++)
	{
		std::cout << c->toString() << std::endl;
	}

	g_outputFile.close();

    char tempCharacter; std::cin >> tempCharacter;
}
