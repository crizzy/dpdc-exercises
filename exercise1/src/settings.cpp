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
#define DATA_PATH "../data/uniprot_20rows.tsv"
#else
#define DATA_PATH "/Users/Markus/Development/C++/dpdc-exercises/exercise1/data/uniprot_20000rows.tsv"
#include <inttypes.h>
#endif

#define VERBOSE
const float searchDensity = 0.01; // the bigger the more we throw out

