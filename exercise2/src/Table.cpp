#include "Table.h"

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <mutex>
#include "Dictionary.h"

Table::Table(Dictionary *dictionary, int tableId)
{
	m_rowCount = -1;
	m_tableId = tableId;

	if (dictionary == 0)
	{
		//Create a local dictionary for this table
		m_dictionary = new Dictionary();
	}
	else
	{
		//Use the dictionary given by the parent
		m_dictionary = dictionary;
	}
}


Table::~Table(void)
{
}


bool Table::readFromFile(std::string fileName)
{
	std::ifstream dataFile(fileName);

	if (!dataFile.is_open())
		return false;

	std::string inputLine;
	size_t left = 0, right = 0;
	int rowIndex, columnIndex;

	// First, determine the number of columns:
	std::getline(dataFile, inputLine, '\n');
	m_columnCount = std::count(inputLine.begin(), inputLine.end(), '\t') + 1;

	// Create column objects:
	for (int columnIndex = 0; columnIndex < m_columnCount; columnIndex++)
	{
		push_back(Column(m_tableId, columnIndex));
	}

	// Jump to the beginning of the file:
	dataFile.seekg(0);

	for (rowIndex = 0; !dataFile.eof(); rowIndex++)
	{
		// Read next input line:
		std::getline(dataFile, inputLine, '\n');

		if (dataFile.eof())
			break;

		left = 0;
		for (columnIndex = 0; (right = inputLine.find('\t', left)) != std::string::npos && columnIndex < m_columnCount; columnIndex++) {
			appendValue(inputLine.substr(left, right-left), columnIndex);
			left = right+1;
		}
		appendValue(inputLine.substr(left, std::string::npos), columnIndex);
	}

	m_rowCount = rowIndex;

	return true;
}

std::mutex dictionaryMutex;

void Table::appendValue(std::string &value, int columnIndex)
{
	dictionaryMutex.lock();
	Dictionary::iterator dictionaryEntry = m_dictionary->find(value);
	if (dictionaryEntry == m_dictionary->end())
	{
		// value has not been found in this column
		unsigned int dictionarySize = m_dictionary->size();
		(*m_dictionary)[value] = dictionarySize;
		dictionaryMutex.unlock();
		this->operator[](columnIndex).appendValue(value, dictionarySize);
	}
	else
	{
		dictionaryMutex.unlock();
		// value is already part of the column
		this->operator[](columnIndex).appendValue(value, dictionaryEntry->second);
	}
}