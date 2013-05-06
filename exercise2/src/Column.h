#ifndef COLUMN_H_DPDC
#define COLUMN_H_DPDC

#include <set>
#include <vector>

class Column : public std::set<int>
{
public:
	Column(int tableId, int columnId);
	~Column(void);

	enum Attribute
	{
		Type = 0,
		Length = 1,
		Case = 2
	};

	void appendValue(std::string &value, unsigned int indexInGlobalDictionary);

	inline int attributeValue(int attributeId) {return m_commonAttributes[attributeId];}
	inline const int attributeCount() const {return 3;}

	inline int tableId() {return m_tableId;}
	inline int columnId() {return m_columnId;}

private:

	std::vector<int> m_commonAttributes;
	//Index 0: Common type
	//Index 1: Common value length
	//Index 2: Common case usage

	int determineValueType(std::string &value);
	int determineValueLength(std::string &value);
	int determineCase(std::string &value);

	int m_tableId;
	int m_columnId;
};

#endif COLUMN_H_DPDC