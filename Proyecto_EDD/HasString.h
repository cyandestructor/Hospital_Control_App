#ifndef HAS_STRING_H
#define HAS_STRING_H

#include <string.h>
#include <fstream>

void WriteWstring(std::wstring& wstr, std::ostream& os);

std::wstring ReadWstring(std::istream& is);

#endif // !HAS_STRING_H
