#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <queue>
#include <set>
#include <thread>
#include <mutex>
#include "Table.h"
#include "Dictionary.h"

std::string DATA_FOLDER_PATH =
    #ifdef _WIN32
        "D:/Daten/dbtesma/";
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

typedef std::unordered_map<int, int> MappingsList;

MappingsList::iterator mapIt;

void checkFunctionalDependency(Column *dependent, Column *referenced, MappingsList *mappingsList)
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

     std::unordered_map<int,int> values; // TODO use hashmap
    
	// disabled to due 5 seconds overhead; maybe try using a hashmap instead of a set here?
    if (!mappingsList->empty())
    {
        //this means we already compared the other way B->A, if we are now doing A->B

   
        
        
        
        
        for(std::pair<int,int> mapping : *mappingsList)
        {
            mapIt = values.find(mapping.second);
            
            if (mapIt == values.end()) // false, if element already existed in set
            {
                // doesnt exist
                
                values[mapIt->second] = mapIt->first;
                //std::cout << "finish B->A early" << std::endl;
                
            }
            else
                return;
            
        }
        mappingsList = &values;
    }
    
    
    

    
	Column::iterator dep = dependent->begin();
	Column::iterator ref = referenced->begin();

	while (dep != dependent->end())
	{

		MappingsList::iterator mappingsEntry = mappingsList->find(*dep);
        if (mappingsEntry == mappingsList->end())
        {
            // value has not been found in this column
            (*mappingsList)[*dep] = *ref;
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

    auto mappingsList = std::unordered_map<int, int>();
    
	checkFunctionalDependency(first, second, &mappingsList);
	mappingsList.clear();
    checkFunctionalDependency(second, first, &mappingsList);
    //mappingsList.clear();

}

Table *table;

void readerWorker()
{
	Dictionary globalDictionary;
	std::ifstream fileStream(inputFileName, std::ios::in | std::ios::binary);
    table = new Table(&globalDictionary);
	table->readFromStream(&fileStream);
}

int g_rightColumn = 0;
std::mutex g_rightColumnMutex;

void computationWorker()
{
	int rightColumn, leftColumn;

    // last step: find inclusion dependencies in columns of the same table:
	g_rightColumnMutex.lock();
    for (rightColumn = g_rightColumn++; rightColumn < table->size(); rightColumn = g_rightColumn++)
    {
		g_rightColumnMutex.unlock();
        //std::cout << "rightColumn: " << rightColumn << std::endl;
        for (leftColumn = rightColumn + 1; leftColumn < table->size(); leftColumn++)
        {
            checkInclusionDependenciesInBothDirections(((*table)[leftColumn]), ((*table)[rightColumn]));
        }
        g_rightColumnMutex.lock();
    }
	g_rightColumnMutex.unlock();
}

int main(int argc, const char *argv[])
{
	time_t start_time = clock();

	unsigned int threadCount = std::thread::hardware_concurrency();
	std::cout << "Looking for functional dependencies using " << threadCount << " threads." << std::endl;

    readerWorker();
    std::cout << "Reading done in: " << timeToString(clock() - start_time) << "." << std::endl;
	std::vector<std::thread> computationThreads;
	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
		computationThreads.push_back(std::thread(computationWorker));
   	for (unsigned int i = 0; i < std::thread::hardware_concurrency(); i++)
		computationThreads[i].join();

	resultsFile.close();

	std::cout << "Done! Total time passed: " << timeToString(clock() - start_time) << "." << std::endl;

	//char c; std::cin >> c;

	return 0;
}