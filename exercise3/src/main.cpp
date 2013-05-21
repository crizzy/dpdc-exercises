#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <queue>
#include <thread>
#include <mutex>
#include "Table.h"
#include "Dictionary.h"

std::string DATA_FOLDER_PATH =
    #ifdef _WIN32
        "???";
    #else
        "/Users/Markus/Development/C++/dpdc-exercises/exercise3/data/";
    #endif

std::string fileNameWithoutEnding = "fd";

std::string inputFileName = DATA_FOLDER_PATH + fileNameWithoutEnding + "-input.tsv";

std::ofstream resultsFile(DATA_FOLDER_PATH + fileNameWithoutEnding + ".tsv");
std::mutex resultsFileMutex;

const std::string timeToString(const time_t readingTime)
{
    time_t ms = 1000 * readingTime / CLOCKS_PER_SEC;
    
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(2) << (ms / 3600000) << ":";
	ss << std::setfill('0') << std::setw(2) << ((ms % 3600000) / 60000) << ":";
	ss << std::setfill('0') << std::setw(2) << std::setprecision(3) << std::fixed << ((ms % 60000) / 1000) << ".";
	ss << std::setfill('0') << std::setw(3) << (ms % 1000);
	return ss.str();
}

void checkFunctionalDependency(Column *dependent, Column *referenced)
{
//    std::cout << dependent->columnId() << ": ";
//    for(int i : *dependent)
//        std::cout << i << ", " ; //TODO columns are empty here:(
//    std::cout << std::endl;
//    std::cout << referenced->columnId() << ": ";
//    for(int i : *referenced)
//        std::cout << i << ", " ; //TODO columns are empty here:(
//    std::cout << std::endl;
//    std::cout << "----------------" << std::endl;
    
    
    // this mappingsList exist just for the comparison of those 2 columns
    typedef std::unordered_map<int, int> MappingsList;
    auto mappingsList = std::unordered_map<int, int>();
    
	Column::iterator dep = dependent->begin();
	Column::iterator ref = referenced->begin();

	while (dep != dependent->end())
	{

		MappingsList::iterator mappingsEntry = mappingsList.find(*dep);
        if (mappingsEntry == mappingsList.end())
        {
            // value has not been found in this column
            mappingsList[*dep] = *ref;
        }
        else
        {
            if (mappingsEntry->second != *ref)
                return;
        }
        dep++;
        ref++;
    }
    
	// Found FD!
	resultsFileMutex.lock();
	resultsFile  << "c" << std::setfill('0') << std::setw(3) << dependent->columnId() << "\t"
                << "c" << std::setfill('0') << std::setw(3) << referenced->columnId() << "" << std::endl;
	resultsFileMutex.unlock();
}

void checkInclusionDependenciesInBothDirections(Column *first, Column *second)
{
    checkFunctionalDependency(second, first);
	checkFunctionalDependency(first, second);
}

struct FileData
{
	std::ifstream *dataStream;
	unsigned int tableId;
	unsigned int tableIndex;
	Table *table;
};
//std::queue<FileData*> fileQueue;
//std::mutex fileQueueMutex;

std::ifstream inputFile;

void readerWorker()
{
    // Get file sizes:
    std::ifstream fileStream(inputFileName, std::ios::in | std::ios::binary);
    
    fileStream.seekg(0, fileStream.end); // set iterator to the end
    size_t fileLength = fileStream.tellg();
    std::cout << "The file length: " << fileLength << std::endl;
    
    
   	//unsigned int tableIndex = 0;

    // Read file stream:
    //FileData *fileData = new FileData;
    inputFile = std::ifstream(inputFileName);
    
//    fileData->dataStream = new std::stringstream;
//    (*fileData->dataStream) << file.rdbuf();
//    fileData->tableId = std::get<2>(fileName);
//    fileData->tableIndex = tableIndex++;
}

//std::queue<FileData*> tableQueue;
//std::mutex tableQueueMutex;
//Table *g_tables[FILE_COUNT];

FileData currentFileData;
Table *table;

void preprocessingWorker()
{
	Dictionary globalDictionary;
	
    int tableId = -1;
    table = new Table(&globalDictionary, tableId);
    currentFileData.dataStream = &inputFile;
    currentFileData.tableId = tableId;
    currentFileData.table = table;
    
    
    currentFileData.table->readFromStream(currentFileData.dataStream);
}

void computationWorker()
{
	//FileData *currentFileData;
	Table *rightTable;
    rightTable = table;
	//unsigned int leftTableIndex, rightTableIndex;
	int rightColumn, leftColumn;

    // last step: find inclusion dependencies in columns of the same table:
    for (rightColumn = 0; rightColumn < rightTable->size(); rightColumn++)
    {
        std::cout << "rightColumn: " << rightColumn << std::endl;
        for (leftColumn = rightColumn + 1; leftColumn < rightTable->size(); leftColumn++)
        {
            checkInclusionDependenciesInBothDirections(((*rightTable)[leftColumn]), ((*rightTable)[rightColumn]));
        }
        
    }
}

int main(int argc, const char *argv[])
{
	time_t start_time = clock();

	unsigned int threadCount = std::thread::hardware_concurrency();
	std::cout << "Looking for functional dependencies using " << threadCount << " threads." << std::endl;

	// Create worker threads:
//	std::thread readerThread(readerWorker);
//	std::vector<std::thread> computationWorkers;
//	for (unsigned int threadId = 0; threadId < threadCount-2; threadId++)
//		computationWorkers.push_back(std::thread(computationWorker));
//	std::thread preprocessingThread(preprocessingWorker);

    readerWorker();
    preprocessingWorker();
    std::cout << "Reading done in: " << timeToString(clock() - start_time) << "." << std::endl;
    computationWorker();
    
    
	// Wait for worker threads:
//	readerThread.join();
//	preprocessingThread.join();
//	for (unsigned int threadId = 0; threadId < threadCount-2; threadId++)
//		computationWorkers[threadId].join();

	resultsFile.close();

	std::cout << "Done! Total time passed: " << timeToString(clock() - start_time) << "." << std::endl;

	//char c; std::cin >> c;

	return 0;
}