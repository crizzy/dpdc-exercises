#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <queue>
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
	
    if (dependent->size() == referenced->size())
    {
        resultsFileMutex.lock();
        resultsFile << "t" << std::setfill('0') << std::setw(3) << referenced->tableId()
        << "[c" << std::setfill('0') << std::setw(3) << referenced->columnId()
        << "]\tt" << std::setfill('0') << std::setw(3) << dependent->tableId()
        << "[c" << std::setfill('0') << std::setw(3) << dependent->columnId() << "]" << std::endl;
        resultsFileMutex.unlock();
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

struct FileData
{
	std::stringstream *dataStream;
	unsigned int tableId;
	unsigned int tableIndex;
	Table *table;
};
std::queue<FileData*> fileQueue;
std::mutex fileQueueMutex;

void readerWorker()
{
	std::set<std::tuple<std::streamoff, std::string, int>> fileNames;// file names sorted by file size

	// Get file sizes:
	for (int tableId = 0; tableId < FILE_COUNT; tableId++)
	{
		std::stringstream fileName;
		fileName << PDB_FOLDER_PATH + "t" << std::setfill('0') << std::setw(3) << tableId << ".tsv";
		std::ifstream fileStream(fileName.str(), std::ios::in | std::ios::binary);
		fileStream.seekg(0, fileStream.end);
		fileNames.insert(std::make_tuple(fileStream.tellg(), fileName.str(), tableId));
	}

	unsigned int tableIndex = 0;
	for (auto &fileName : fileNames)
	{
		// Read file stream:
		FileData *fileData = new FileData;
		std::ifstream file(std::get<1>(fileName));
		fileData->dataStream = new std::stringstream;
		(*fileData->dataStream) << file.rdbuf();
		fileData->tableId = std::get<2>(fileName);
		fileData->tableIndex = tableIndex++;

		// Make file stream available to the next pipeline worker:
		fileQueueMutex.lock();
		fileQueue.push(fileData);
		fileQueueMutex.unlock();
	}

	//Signal completion by pushing a null object:
	fileQueueMutex.lock();
	fileQueue.push(0);
	fileQueueMutex.unlock();
}

std::queue<FileData*> tableQueue;
std::mutex tableQueueMutex;
Table *g_tables[FILE_COUNT];

void preprocessingWorker()
{
	Dictionary globalDictionary;
	FileData *currentFileData;

	while (true)
	{
		// Fetch next file stream:
		fileQueueMutex.lock();
		while (fileQueue.empty())
		{
			fileQueueMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			fileQueueMutex.lock();
		}
		currentFileData = fileQueue.front();
		if (currentFileData == 0)// Finished?
		{
			fileQueueMutex.unlock();
			break;
		}
		fileQueue.pop();
		fileQueueMutex.unlock();

		// Preprocess file stream:
		currentFileData->table = new Table(&globalDictionary, currentFileData->tableId);
		currentFileData->table->readFromStream(currentFileData->dataStream);
		g_tables[currentFileData->tableIndex] = currentFileData->table;

		// Make preprocessed data available for next pipeline worker:
		tableQueueMutex.lock();
		tableQueue.push(currentFileData);
		tableQueueMutex.unlock();
	}

	//Signal completion by pushing a null object:
	tableQueueMutex.lock();
	tableQueue.push(0);
	tableQueueMutex.unlock();
}

void computationWorker()
{
	FileData *currentFileData;
	Table *rightTable;
	unsigned int leftTableIndex, rightTableIndex;
	int rightColumn, leftColumn;

	while (true)
	{
		// Fetch next table:
		tableQueueMutex.lock();
		while (tableQueue.empty())
		{
			tableQueueMutex.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			tableQueueMutex.lock();
		}
		currentFileData = tableQueue.front();
		if (currentFileData == 0)// Finished?
		{
			tableQueueMutex.unlock();
			break;
		}
		tableQueue.pop();
		tableQueueMutex.unlock();

		// First step: search all column combinations (this table with other tables)
		rightTableIndex = currentFileData->tableIndex;
		rightTable = currentFileData->table;

		for (leftTableIndex = 0; leftTableIndex < rightTableIndex; leftTableIndex++)
		{
			for (rightColumn = 0; rightColumn < rightTable->size(); rightColumn++)
			{
				for (leftColumn = 0; leftColumn < g_tables[leftTableIndex]->size(); leftColumn++)
				{
					checkInclusionDependenciesInBothDirections(&((*g_tables[leftTableIndex])[leftColumn]), &((*rightTable)[rightColumn]));
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

	unsigned int threadCount = std::thread::hardware_concurrency();

	std::cout << "Looking for inclusion dependencies using " << threadCount << " threads." << std::endl;

	// Create worker threads:
	std::thread readerThread(readerWorker);
	std::vector<std::thread> computationWorkers;
	for (unsigned int threadId = 0; threadId < threadCount-2; threadId++)
		computationWorkers.push_back(std::thread(computationWorker));
	std::thread preprocessingThread(preprocessingWorker);

	// Wait for worker threads:
	readerThread.join();
	preprocessingThread.join();
	for (unsigned int threadId = 0; threadId < threadCount-2; threadId++)
		computationWorkers[threadId].join();

	resultsFile.close();

	std::cout << "Done! Total time passed: " << timeToString(clock() - start_time) << "." << std::endl;

	char c; std::cin >> c;

	return 0;
}