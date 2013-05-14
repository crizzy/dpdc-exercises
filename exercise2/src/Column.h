#ifndef COLUMN_H_DPDC
#define COLUMN_H_DPDC

#include <set>
#include <vector>

class Column : public std::set<int>
{
public:
	Column(int tableId, int columnId);
	~Column(void);

	void appendValue(unsigned int indexInGlobalDictionary);


	inline int tableId() {return m_tableId;}
	inline int columnId() {return m_columnId;}

private:
	int m_tableId;
	int m_columnId;
};

#endif //COLUMN_H_DPDC
