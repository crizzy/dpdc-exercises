#include "Column.h"

#include <algorithm>
#include <sstream>

inline bool isDigit(char c) {return c >= '0' && c <= '9';}
inline bool isNonDigit(char c) {return c < '0' || c > '9';}
inline bool isSymbolOfRealNumber(char c) {return isDigit(c) || c == '.' || c == 'E';}
inline bool isSpace(char c) {return c == 0x20;}
inline bool isDot(char c) {return c == '.';}
inline bool isUpperCaseLetter(char c) {return c >= 'A' && c <= 'Z';}
inline bool isLowerCaseLetter(char c) {return c >= 'a' && c <= 'z';}

Column::Column(int tableId, int columnId)
{
	m_tableId = tableId;
	m_columnId = columnId;

	for (int i = 0; i < attributeCount(); i++)
		m_commonAttributes.push_back(-2);
}


Column::~Column(void)
{
}

void Column::appendValue(std::string &value, unsigned int indexInGlobalDictionary)
{
	// Don't save the actual value, but only the index in the dictionary:
	insert(indexInGlobalDictionary);

	// Check type:
	/*if (m_commonAttributes[0] == -2)
		m_commonAttributes[0] = determineValueType(value);
	else if (m_commonAttributes[0] != -1 && m_commonAttributes[0] != determineValueType(value))
		m_commonAttributes[0] = -1;

	// Check length:
	if (m_commonAttributes[1] == -2)
		m_commonAttributes[1] = determineValueLength(value);
	else if (m_commonAttributes[1] != -1)
		if (m_commonAttributes[1] != determineValueLength(value))
			m_commonAttributes[1] = -1;

	// Check case:
	if (m_commonAttributes[2] == -2)
		m_commonAttributes[2] = determineCase(value);
	else if (m_commonAttributes[2] != -1)
		if (m_commonAttributes[2] != determineCase(value))
			m_commonAttributes[2] = -1;*/
}

int Column::determineValueType(std::string &value)
{
	// Does the value only consist of 0..9?
	if (std::count_if(value.begin(), value.end(), isDigit) == value.length())
		return 0;// UINT

	// Does the value only constist of 0..9 and exactly one radix point (.)?
	if (std::count_if(value.begin(), value.end(), isSymbolOfRealNumber) == value.length() && std::count_if(value.begin(), value.end(), isDot) == 1 && value.back() != 'E')
		return 1;// REAL

	return 2;// STRING
}

int Column::determineValueLength(std::string &value)
{
	return std::min<int>(value.length(), 16);
}

int Column::determineCase(std::string &value)
{
	// Are all characters upper cased?
	if (std::count_if(value.begin(), value.end(), isLowerCaseLetter) == 0)
		return 0;// UPPER CASE

	// Are all characters lower cased?
	if (std::count_if(value.begin(), value.end(), isUpperCaseLetter) == 0)
		return 1;// LOWER CASE

	return -1;// MIXED
}