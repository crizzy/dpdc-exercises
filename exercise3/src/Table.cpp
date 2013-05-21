#include "Table.h"

#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <mutex>
#include "Dictionary.h"

Table::Table(Dictionary *dictionary)
{
	m_rowCount = -1;
	m_dictionary = dictionary;
}

Table::~Table(void)
{
}

bool Table::readFromStream(std::istream *dataStream)
{
	std::string inputLine;
	size_t left = 0, right = 0;
	unsigned int rowIndex, columnIndex;

	// First, determine the number of columns:
	std::getline(*dataStream, inputLine, '\n');
	m_columnCount = (unsigned int)(std::count(inputLine.begin(), inputLine.end(), '\t') + 1);

	// Create column objects:
	for (columnIndex = 0; columnIndex < m_columnCount; columnIndex++)
	{
		push_back(new Column(columnIndex));
	}

	// Jump to the beginning of the file:
	dataStream->seekg(0);

	for (rowIndex = 0; !dataStream->eof(); rowIndex++)
	{
        //if (rowIndex % 1000 == 0)
        //    std::cout << "Reading row: " << rowIndex << std::endl;
		// Read next input line:
		std::getline(*dataStream, inputLine, '\n');

		if (dataStream->eof())
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

void Table::appendValue(const std::string value, const int columnIndex)
{
	Dictionary::iterator dictionaryEntry = m_dictionary->find(value);
	if (dictionaryEntry == m_dictionary->end())
	{
		// value has not been found in this column
		unsigned int dictionarySize = (unsigned int)m_dictionary->size();
		(*m_dictionary)[value] = dictionarySize;
		this->operator[](columnIndex)->push_back(dictionarySize);
	}
	else
	{
		// value is already part of the column
		this->operator[](columnIndex)->push_back(dictionaryEntry->second);
	}
}