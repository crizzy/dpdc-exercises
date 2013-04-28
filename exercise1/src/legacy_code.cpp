

/*std::vector<std::vector<int>> isUniqueColumnCombinationPLI(ColumnCombination &combination) // Markus //TODO rename method // for comb with size 2
 {
 //if result vector is empty than it was unique
 
 if (combination.size() != 2)
 {
 std::cout << "isUniqueColumnCombinationPLI() assumes combination of size 2" << std::endl;
 return std::vector<std::vector<int>>();
 }
 std::vector<std::vector<int>> duplicates = std::vector<std::vector<int>>(); // AB = {{r1,r3},{},{}}
 
 std::vector<std::vector<std::vector<int>>> vec_of_columnDuplicates; // A: {{r1, r2, r3}, {r4, r5}}; B: ...
 vec_of_columnDuplicates.push_back(std::vector<std::vector<int>>()); // A: {{r1, r2, r3}, {r4, r5}} für ersten Durchlauf
 vec_of_columnDuplicates.push_back(std::vector<std::vector<int>>());
 
 for (int combIndex = 0; combIndex < combination.size(); combIndex++)
 {
 // this preprocesses 1 column (1. PLI slide)
 
 int columnIndex = combination[combIndex];
 std::map<int,std::vector<int>> dict; // A: a->{r1, r2, r3}, b->{r4, r5}
 
 std::vector<int> &column = g_columns[columnIndex];
 for (int rowIndex = 0; rowIndex < column.size(); rowIndex++)
 {
 int &cell = column[rowIndex];
 std::map<int,std::vector<int>>::iterator it = dict.find(cell);
 if (it == dict.end()) // key not existing yet
 {
 // build new vector with current row in it
 std::vector<int> vec = std::vector<int>();
 vec.push_back(rowIndex);
 dict[cell] = vec;
 }
 else // key existing
 {
 it->second.push_back(rowIndex);
 }
 }
 
 // iterate over map, throw out unique values (where vector has just 1 element), and put remaining vectors in a vector
 std::map<int,std::vector<int>>::iterator it;
 for (it=dict.begin(); it!=dict.end(); it++)
 {
 if (it->second.size() > 1)
 vec_of_columnDuplicates[combIndex].push_back(it->second); //assume it->second is sorted
 }
 
 }
 
 //pairwise intersection
 std::vector<std::vector<int>>::iterator it1; // iterator for column A
 std::vector<std::vector<int>>::iterator it2; // iterator for column B
 for (it1 = vec_of_columnDuplicates[0].begin(); it1 < vec_of_columnDuplicates[0].end(); ++it1)
 {
 for (it2 = vec_of_columnDuplicates[1].begin(); it2 < vec_of_columnDuplicates[1].end(); ++it2)
 {
 time_t beginTimeIntersection = clock();
 
 std::vector<int> v(it1->size() + it2->size()); //possible without making new vector
 // we assume that (*it1) is sorted
 std::vector<int>::iterator it = std::set_intersection(it1->begin(), it1->end(), it2->begin(), it2->end(), v.begin());// takes a lot of time
 v.resize(it - v.begin());
 
 if (v.size() > 1)
 {
 duplicates.push_back(v);
 //return duplicates; // this makes intersection time shrink a lot, but results can't be propagated
 }
 
 time_t endTimeIntersection = clock();
 g_timeForIntersection += (float(endTimeIntersection - beginTimeIntersection)) / CLOCKS_PER_SEC;
 }
 }
 return duplicates;
 }*/



/*bool checkUniquenessFor1Column(std::vector<int> &column)
 {
 size_t sizeBefore = column.size();
 ColumnVector::iterator it = std::unique(column.begin(), //TODO: do this for multiple columns
 column.end());  //TODO: end early as soon as two rows are the same -> not done by unique
 size_t sizeAfter = it - column.begin();
 
 if (sizeAfter == sizeBefore)
 return true;
 return false;
 }
 
 bool isUniqueColumnCombination(std::vector<int> &combination)
 {//TODO make it work
 
 //print out what we are looking for
 std::cout << "looking for a UCC in the combination {";
 for (int i = 0; i< combination.size() - 1; i++)
 std::cout << combination[i] << ", ";
 std::cout << *(combination.end()-1) << "}" << std::endl;
 
 if (combination.size() > 1)
 {
 std::vector<std::string> vec_stichedColumn;
 for (int rowIndex = 0; rowIndex < g_columns[0].size(); rowIndex++)
 {
 std::stringstream sstr_stichedColumn(g_columns[combination.at(0)].at(rowIndex));
 for (int i = 1; i < combination.size(); i++)
 sstr_stichedColumn << "/t" << g_columns[combination.at(i)].at(rowIndex);
 vec_stichedColumn.push_back(sstr_stichedColumn.str());
 }
 return checkUniquenessFor1Column(vec_stichedColumn);
 
 }
 else
 {
 std::vector<std::string> &column = g_columns[combination[0]];
 return checkUniquenessFor1Column(column);
 //TODO {1, 2} sind noch false
 }
 }*/