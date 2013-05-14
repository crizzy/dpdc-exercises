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