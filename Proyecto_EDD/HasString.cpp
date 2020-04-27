#include "HasString.h"

void WriteWstring(std::wstring& wstr, std::ostream& os) {
	size_t len = wstr.length();
	os.write(reinterpret_cast<char*>(&len), sizeof(size_t));							//write the length
	os.write(reinterpret_cast<char*>(&wstr[0]), (long long)len * sizeof(wchar_t));		//write the content
}

std::wstring ReadWstring(std::istream& is) {

	std::wstring tmp;
	size_t len = 0;
	wchar_t* buffer;

	is.read(reinterpret_cast<char*>(&len), sizeof(size_t));								//read the length
	buffer = new wchar_t[len + 1];
	is.read(reinterpret_cast<char*>(buffer), (long long)len * sizeof(wchar_t));			//read the content
	buffer[len] = 0; //\0
	tmp = buffer;
	delete[] buffer;

	return tmp;

}