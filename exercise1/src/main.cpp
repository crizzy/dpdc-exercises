#include "settings.cpp"


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

	int maxIndex() // schliesst aus das wir keine Kombination doppelt machen TODO check if too much is pruned
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

typedef std::map<std::string, int> Dictionary;

// global vars
std::vector<ColumnVector> g_columns;//all single columns
std::vector<ColumnVector> g_combinedColumns[2];//column combinations
std::vector<ColumnCombination> g_results;
std::ofstream g_outputFile;
std::map<int, int> g_goodColumns;

long long g_columnCount = 0;

std::map<int, ColumnCombination> g_costs;
int g_rowCount = 1000000000;//important: must be initialized to artificially high value

float g_timeForIntersection;
float g_timeTotal;

#define BUFFER_SIZE 128000

std::vector<ColumnCombination> buildLikelyCombinations() // hardcoded from 20000 run till {8,14} with {175}
{
    std::vector<ColumnCombination> combinations;
    
	// read column names:
	std::ifstream I(RESULT_PATH);
	if (!I.is_open())
	{
		std::cout << "Cannot find results file." << std::endl;
		return std::vector<ColumnCombination>() ;
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
            if (colIndex==0)
            {
                ColumnCombination combination = ColumnCombination();
                combinations.push_back(combination);
            }
			s_rowBuffer.getline(cellBuffer, BUFFER_SIZE, '\t');
            int combinationElement = atoi(cellBuffer);
			combinations.at(rowIndex).push_back((combinationElement));
			std::cout << cellBuffer << "\t";
		}    
		std::cout << std::endl;
    }
    
    return combinations;
}

int checkLikelyCombinations()
{
    
    // read in combination from result file
    std::vector<ColumnCombination> combinations = buildLikelyCombinations();
/////////////////////
    
    // read in strings of data file
    std::vector<std::vector<std::string>> columns;

	for (int i = 0; i<223; i++)
	{
        std::vector<std::string> column = std::vector<std::string>();
		columns.push_back(column);
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
			//std::cout << cellBuffer << "\t";
		}
		//std::cout << std::endl;
		if (rowIndex % 54000 == 0)
			std::cout << "read " << rowIndex << " rows." << std::endl;
        
	}
    

	std::cout << "Finished reading strings data file!" << std::endl;
    
    //check combinations
    
    for (int combIndex = 0; combIndex < combinations.size(); combIndex++)
    {
        std::vector<std::string> columnsConcatenated;
        for (int rowIndex = 0; rowIndex < columns[0].size(); rowIndex++)
        {
            std::stringstream rowString;
            for (int colIndex = 0; colIndex < combinations[combIndex].size(); colIndex++)
            {
                int columnConsidered = combinations[combIndex][colIndex];
                rowString << columns[columnConsidered][rowIndex] << "/t";
            }
            
            columnsConcatenated.push_back(rowString.str());
        }
        
        // check if all strings are unique
        
        size_t sizeBefore = columnsConcatenated.size();
        std::vector<std::string>::iterator it;
        it = std::unique (columnsConcatenated.begin(), columnsConcatenated.end());
        columnsConcatenated.resize( std::distance(columnsConcatenated.begin(),it) );
        size_t sizeAfter = columnsConcatenated.size();
        if (sizeBefore == sizeAfter)
            std::cout << combinations[combIndex].toString() << " is unique!!!!!!!!!!" << std::endl;
        else
            std::cout << combinations[combIndex].toString() << " is not unique." << std::endl;
        columnsConcatenated.clear();

    }
    std::cout << "Checking done." << std::endl;
    // wait
    char c; std::cin >> c;
    return 0;
}




int getNumberOfUniques(ColumnVector &cv)
{
	int uniques = g_rowCount;//??? funktioniert doch nur fuer 2 oder?
	for (ColumnVector::iterator set = cv.begin(); set != cv.end(); ++set)
	{
		uniques -= (int)set->size();
	}
	return uniques; // give back number of sets
}

void reportUniqueColumnCombination(ColumnCombination &c)
{
	#ifdef VERBOSE
		std::cout << " => Dropped, because column combination is unique: " << c.toString();
	#endif
	g_results.push_back(c);
	
    // open
    g_outputFile.open(std::string(DATA_PATH) + std::string("_uniques.txt"), std::ios::out | std::ios::app);
	if (!g_outputFile.is_open())
	{
		std::cout << "Cannot write to output file \"" << std::string(DATA_PATH) << "_uniques.txt\"" << std::endl;
		return;
	}
    
    // write
    g_outputFile << c.toTabbedString() << std::endl;
    
    // close
    g_outputFile.close();
}

std::string timeToString(time_t readingTime)
{
    time_t ms = float(readingTime) / CLOCKS_PER_SEC * 1000;
    
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << (ms / 3600000) << ":";
	ss << std::setfill('0') << std::setw(2) << ((ms % 3600000) / 60000) << ":";
	ss << std::setfill('0') << std::setw(2) << std::setprecision(3) << std::fixed << ((ms % 60000) / 1000) << ".";
	ss << std::setfill('0') << std::setw(3) << (ms % 1000);
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
    g_outputFile.close();
    
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
		for (ColumnVector::iterator it = currentColumnVector.begin(); it != currentColumnVector.end(); it++)
		{
			if (it->size() > 1)
				cleanColumnVector.push_back(*it);
		}

		if (colIndex == 0)
			std::cout << "Detected " << (g_rowCount = rowIndex) << " rows." << std::endl;

		// determine number of unique values in this column:
		int uniquesCount = getNumberOfUniques(cleanColumnVector);
        size_t cleanColumnVectorSize = cleanColumnVector.size();
        
		std::cout << "Column " << colIndex << ": " << cleanColumnVectorSize << (cleanColumnVectorSize == 1 ? " set" : " sets") << " and " << uniquesCount << " uniques.";

		if (uniquesCount == g_rowCount)
		{
			// drop column, if only containing unique values:
            ColumnCombination c = ColumnCombination(colIndex);
			reportUniqueColumnCombination(c);
		}
		else if (uniquesCount < g_rowCount * searchDensity)
		{
			// drop column, if there are too few unique values:
			std::cout << " => Dropped because of too many duplicates.";
		}
		else if (g_goodColumns.find(colIndex) == g_goodColumns.end())
		{
			//skip this column
			std::cout << " => Skipped.";
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
    
	std::cout << "Finished reading " << g_columnCount << " columns in " << timeToString(clock() - readStartTime) << "." << std::endl;

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
#ifdef CHECK_RESULT
    checkLikelyCombinations();
    return 0;
#endif
    
    time_t beginTimeTotal = clock();


	g_goodColumns[2];
	g_goodColumns[7];
	g_goodColumns[12];
	g_goodColumns[13];
	g_goodColumns[20];
	g_goodColumns[37];
	g_goodColumns[45];
	g_goodColumns[79];
	g_goodColumns[185];
   
    // read stuff
    readColumnsFromFile();
	printTable();

	std::cout << "Operating on a subset of " << g_columnCount << " columns." << std::endl;
    
	int columnCount = (int)g_columns.size();

	std::vector<int> intersection;
    std::vector<int>::iterator temp_leftSetBegin;
    std::vector<int>::iterator temp_leftSetEnd;
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
                    temp_leftSetBegin = leftSet->begin();
                    temp_leftSetEnd = leftSet->end();
					for (ColumnVector::iterator rightSet = rightColumnVector->begin(); rightSet != rightColumnVector->end(); ++rightSet)
					{
                        std::set_intersection(
							temp_leftSetBegin,
							temp_leftSetEnd,
							rightSet->begin(),
							rightSet->end(),
							std::back_inserter(intersection)
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
				//if (uniques != std::max<int>(leftColumnVector->uniques, rightColumnVector->uniques))
				//{
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
				//}
			}
		}
		std::cout << "\nDuration: " << timeToString(clock() - timeforOneDimension) << std::endl;

		// clear source table:
		if (columnDimension > 2)
			sourceTable->clear();

		// invert roles of source and target table:
		sourceTable = &g_combinedColumns[1 - (columnDimension % 2)];
		targetTable = &g_combinedColumns[columnDimension % 2];
	}

    std::cout << "\n\nDone in " << timeToString(clock() - beginTimeTotal) << ". Found " << g_results.size() << " unique column combination" << (g_results.size() != 1 ? "s" : "") << ": " << std::endl;
	for (std::vector<ColumnCombination>::iterator c = g_results.begin(); c != g_results.end(); c++)
	{
		std::cout << c->toString() << std::endl;
	}

	g_outputFile.close();

    char tempCharacter; std::cin >> tempCharacter;
}


// TODO
// ordered list ordered by complexity
// check union
// dont throw away 3er comb (if(max) auskommentieren)  // why doesnt he do big combinations: he is done after 6...
//do on 20000 file, than check those on big file  //run on small file, push_back
