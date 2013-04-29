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
#include <iomanip>

#ifdef _WIN32
#define DATA_PATH "../data/uniprot.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/uniprot_700rows.tsv"
#include <inttypes.h>
#endif

#ifdef _WIN32
#define RESULT_PATH "../results/uniprot_20000rows.tsv_uniques_density0.01.txt"
//uniprot_20000rows.tsv_uniques_density0.01.txt
//uniprot_20000rows.tsv_uniques_14, 103 with 176_density0.txt
//uniprot_20000rows.tsv_uniques_up to 8-14 with 175 at density 0.0.txt
#else
#define RESULT_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/results/uniprot_20000rows.tsv_uniques_up to 8-14 with 175 at density 0.0.txt"
#endif

//#define CHECK_RESULT


//#define VERBOSE
const float searchDensity = 0.0; // the bigger the more we throw out

