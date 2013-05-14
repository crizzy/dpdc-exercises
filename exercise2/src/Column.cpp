#include "Column.h"

#include <algorithm>
#include <sstream>

Column::Column(int tableId, int columnId)
{
	m_tableId = tableId;
	m_columnId = columnId;
}


Column::~Column(void)
{
}

void Column::appendValue(unsigned int indexInGlobalDictionary)
{
	// Don't save the actual value, but only the index in the dictionary:
	insert(indexInGlobalDictionary);
}

