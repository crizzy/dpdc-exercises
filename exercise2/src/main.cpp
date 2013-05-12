#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include "Table.h"
#include "Dictionary.h"


#define FILE_COUNT 105
#define MAX_TABLE_ID FILE_COUNT-1



std::string PDB_FOLDER_PATH =
    #ifdef _WIN32
        "D:/Daten/pdb/";
    #else
        "/Users/Markus/Development/C++/dpdc-exercises/exercise2/data/pdb/";
    #endif

std::ofstream resultsFile(PDB_FOLDER_PATH + "inds.tsv");
std::mutex resultsFileMutex;

std::string timeToString(time_t readingTime)
{
    time_t ms = 1000 * readingTime / CLOCKS_PER_SEC;
    
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << (ms / 3600000) << ":";
	ss << std::setfill('0') << std::setw(2) << ((ms % 3600000) / 60000) << ":";
	ss << std::setfill('0') << std::setw(2) << std::setprecision(3) << std::fixed << ((ms % 60000) / 1000) << ".";
	ss << std::setfill('0') << std::setw(3) << (ms % 1000);
	return ss.str();
}

void checkInclusionDependency(Column *dependent, Column *referenced)
{
	Column::iterator dep = dependent->begin();
	Column::iterator ref = referenced->begin();

	while (dep != dependent->end())
	{
		if (ref == referenced->end())
			return; //No IND

		if (*dep == *ref)
		{
			dep++;
			ref++;
			continue;
		}
		if (*dep < *ref)
		{
			return;//No IND
		}
		//if *dep > *ref:
		do {
			ref++;

			if (ref == referenced->end())
				return;//No IND
		} while (*dep > *ref);

		if (*dep != *ref)
			return;//No IND	
	}
	
	// Found IND!
	resultsFileMutex.lock();
	resultsFile << "t" << std::setfill('0') << std::setw(3) << dependent->tableId()
	<< "[c" << std::setfill('0') << std::setw(3) << dependent->columnId()
	<< "]\tt" << std::setfill('0') << std::setw(3) << referenced->tableId()
	<< "[c" << std::setfill('0') << std::setw(3) << referenced->columnId() << "]" << std::endl;
	resultsFileMutex.unlock();
}

void checkInclusionDependenciesInBothDirections(Column *first, Column *second)
{
	if (first->size() > second->size())
		return checkInclusionDependency(second, first);
	return checkInclusionDependency(first, second);
}

unsigned int g_tablesChecked = 0;
std::mutex g_tablesCheckedMutex;
Table *g_tables[FILE_COUNT];
unsigned int g_tablesRead = 0;
std::mutex g_tablesReadMutex;

void findInclusionDependencies()
{

	Table *rightTable;
	unsigned int leftTableId, rightTableId;
	int rightColumn, leftColumn;

	while (true)
	{
		g_tablesCheckedMutex.lock();
		rightTableId = g_tablesChecked++;
		g_tablesCheckedMutex.unlock();

		if (rightTableId > MAX_TABLE_ID)
			return;

		g_tablesReadMutex.lock();
		while (rightTableId >= g_tablesRead)
		{
			g_tablesReadMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			g_tablesReadMutex.lock();
		}
		g_tablesReadMutex.unlock();

		rightTable = g_tables[rightTableId];

		for (leftTableId = 0; leftTableId < rightTableId; leftTableId++)
		{
			for (rightColumn = 0; rightColumn < rightTable->size(); rightColumn++)
			{
				for (leftColumn = 0; leftColumn < g_tables[leftTableId]->size(); leftColumn++)
				{
					checkInclusionDependenciesInBothDirections(&((*g_tables[leftTableId])[leftColumn]), &((*rightTable)[rightColumn]));
				}
			}
		}

		// last step: find inclusion dependencies in columns of the same table:
		for (rightColumn = 0; rightColumn < rightTable->size(); rightColumn++)
		{
			for (leftColumn = rightColumn + 1; leftColumn < rightTable->size(); leftColumn++)
			{
				checkInclusionDependenciesInBothDirections(&((*rightTable)[leftColumn]), &((*rightTable)[rightColumn]));
			}
		}
	}
}

int main(int argc, const char *argv[])
{
	time_t start_time = clock();
	Dictionary globalDictionary;
	unsigned int threadCount = std::thread::hardware_concurrency();
	unsigned int tableId;
	std::set<std::pair<std::streamoff, std::string>> fileNames;// file names sorted by file size

	std::cout << "Looking for inclusion dependencies using " << threadCount << " threads." << std::endl;

	// Get file sizes:
	for (tableId = 0; tableId < FILE_COUNT; tableId++)
	{
		std::stringstream fileName;
		fileName << PDB_FOLDER_PATH + "t" << std::setfill('0') << std::setw(3) << tableId << ".tsv";
		std::ifstream fileStream(fileName.str(), std::ios::in | std::ios::binary);
		fileStream.seekg(0, fileStream.end);
		fileNames.insert(std::make_pair(fileStream.tellg(), fileName.str()));
	}

	// Create worker threads:
	std::vector<std::thread> threads;
	for (unsigned int threadId = 0; threadId < threadCount-1; threadId++)
		threads.push_back(std::thread(findInclusionDependencies));

	// Read all files:
	tableId = 0;
	for (auto &fileName : fileNames)
	{
		time_t time_for_file = clock();

		std::cout << "Reading file " << fileName.second << "...";

		g_tables[tableId] = new Table(&globalDictionary, tableId);
		g_tables[tableId]->readFromFile(fileName.second);
		std::cout << "done! " << g_tables[tableId]->columnCount() << " columns and " << g_tables[tableId]->rowCount() << " rows. Dictionary size: " << globalDictionary.size() << " Time needed: " << timeToString(clock()-time_for_file) << std::endl;

		g_tablesReadMutex.lock();
		g_tablesRead++;
		g_tablesReadMutex.unlock();

		tableId++;
	}

	// Wait for worker threads:
	for (unsigned int threadId = 0; threadId < threadCount-1; threadId++)
		threads[threadId].join();

	resultsFile.close();

	std::cout << "Done! Total time passed: " << timeToString(clock() - start_time) << "." << std::endl;

	char c; std::cin >> c;

	return 0;
}