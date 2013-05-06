#ifndef TABLE_H_DPDC
#define TABLE_H_DPDC

#include <vector>
#include "Column.h"
#include "Dictionary.h"

class Table : public std::vector<Column>
{
public:
	Table(Dictionary *dictionary, int tableId);
	~Table(void);

	bool readFromFile(std::string fileName);

	inline unsigned int columnCount() {return m_columnCount;}
	inline unsigned int rowCount() {return m_rowCount;}

private:

	Dictionary *m_dictionary;

	unsigned int m_columnCount;
	unsigned int m_rowCount;

	int m_tableId;

	inline void appendValue(std::string &value, int columnIndex);

};

#endif TABLE_H_DPDC