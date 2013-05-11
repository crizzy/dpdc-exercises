#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <thread>
#include <mutex>
#include "Table.h"
#include "Dictionary.h"

std::ofstream resultsFile("D:/Daten/pdb/inds.tsv");

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

bool isInclusionDependency(Column *dependent, Column *referenced)
{
	Column::iterator dep = dependent->begin();
	Column::iterator ref = referenced->begin();

	while (dep != dependent->end())
	{
		if (ref == referenced->end())
			return false; //No IND

		if (*dep == *ref)
		{
			dep++;
			ref++;
			continue;
		}
		if (*dep < *ref)
		{
			return false;//No IND
		}
		//if *dep > *ref:
		do {
			ref++;

			if (ref == referenced->end())
				return false;//No IND
		} while (*dep > *ref);

		if (*dep != *ref)
			return false;//No IND	
	}
	return true;
}

int g_leftIndex = 0;
std::mutex g_leftIndexMutex;
std::mutex resultsFileMutex;

void findInclusionDependencies(std::vector<Column*> &columns)
{
	g_leftIndexMutex.lock();
	for (int leftIndex = g_leftIndex++; leftIndex < columns.size(); leftIndex = g_leftIndex++)
	{
		g_leftIndexMutex.unlock();

		for (int rightIndex = leftIndex+1; rightIndex < columns.size(); rightIndex++)
		{
			if (isInclusionDependency(columns[leftIndex], columns[rightIndex]))
			{
				resultsFileMutex.lock();
				resultsFile << "t" << std::setfill('0') << std::setw(3) << columns[leftIndex]->tableId()
				<< "[c" << std::setfill('0') << std::setw(3) << columns[leftIndex]->columnId()
				<< "]\tt" << std::setfill('0') << std::setw(3) << columns[rightIndex]->tableId()
				<< "[c" << std::setfill('0') << std::setw(3) << columns[rightIndex]->columnId() << "]" << std::endl;
				resultsFileMutex.unlock();
			}
		}
		g_leftIndexMutex.lock();
	}
	g_leftIndexMutex.unlock();
}

int main(int argc, const char *argv[])
{
	time_t start_time = clock();

	Dictionary *globalDictionary = new Dictionary;
	std::vector<Table*> tables;
	auto *columnSet = new std::set<std::pair<int, Column*>>();

	Table *t[104];

	std::cout << "Detected " << std::thread::hardware_concurrency() << " threads." << std::endl;

	// Read all files:
	for (int i = 0; i <= 104; i++)
	{
		time_t time_for_file = clock();
		std::stringstream fileName;
		fileName << "D:/Daten/pdb/t" << std::setfill('0') << std::setw(3) << i << ".tsv";
		std::cout << "Read file " << fileName.str() << "...";

		t[i] = new Table(globalDictionary, i);
		t[i]->readFromFile(fileName.str());
		std::cout << "done! " << t[i]->columnCount() << " columns and " << t[i]->rowCount() << " rows. Dictionary size: " << globalDictionary->size() << " Time needed: " << timeToString(clock()-time_for_file) << std::endl;
		//int columnIndex = 0; //unused

		// Sort column into set:
		for (Table::iterator column = t[i]->begin(); column != t[i]->end(); column++)
		{
			columnSet->insert(std::make_pair(column->size(), &(*column)));
		}
	}

	delete globalDictionary;

	// Write columns back into vector:
	std::vector<Column*> columns;
	for (auto &c : *columnSet)
	{
		columns.push_back(c.second);
	}
	delete columnSet;

	std::cout << "Finished reading after " << timeToString(clock() - start_time) << "   Total number of columns: " << columns.size() << std::endl;

	//do computation
	std::vector<std::thread> threads;
	for (int threadId = 0; threadId < std::thread::hardware_concurrency(); threadId++)
		threads.push_back(std::thread(findInclusionDependencies, columns));
	for (int threadId = 0; threadId < std::thread::hardware_concurrency(); threadId++)
		threads[threadId].join();

	resultsFile.close();

	std::cout << "Finished after " << timeToString(clock() - start_time) << "." << std::endl;

	char c; std::cin >> c;

	return 0;
}