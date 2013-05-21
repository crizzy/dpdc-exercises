#ifndef COLUMN_H_DPDC
#define COLUMN_H_DPDC

#include <vector>

class Column : public std::vector<int>
{
public:
	Column(int columnId);
	~Column(void);

	inline int columnId() {return m_columnId;}

private:
	int m_columnId;
};

#endif //COLUMN_H_DPDC
