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

#include <map>

#define BUFFER_SIZE 128000

typedef std::vector<std::string> ColumnVector;

int main(int argc, const char * argv[])
{
	std::vector<ColumnVector> columns;
	for (int i = 0; i<223; i++)
	{
		columns.push_back(ColumnVector());
	}

	// read column names:
	std::ifstream I("../data/uniprot.tsv");
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

		if (rowIndex % 54000 == 0)
			std::cout << rowBuffer << std::endl;

		for (int colIndex = 0; !s_rowBuffer.eof(); colIndex++)
		{
			s_rowBuffer.getline(cellBuffer, BUFFER_SIZE, '\t');
			columns.at(colIndex).push_back(cellBuffer);
			//std::cout << cellBuffer << "\t";
		}
		//std::cout << std::endl;
		if (rowIndex % 540000 == 0)
			std::cout << "read " << rowIndex << " rows." << std::endl;

	}

	char c;
	std::cout << "Finished!" << std::endl;
	std::cin >> c;

    return 0;
}

