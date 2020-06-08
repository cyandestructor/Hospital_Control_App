#ifndef  USER_CLASS
#define USER_CLASS

#include <string>
#include <iostream>

class User {

public:
	friend std::wostream& operator<<(std::wostream& os, const User& user);
	friend std::wistream& operator>>(std::wistream& is, User& user);
	User() {}
	User(const std::wstring& username, const std::wstring& password) {}
	bool CheckUser(const std::wstring& username, const std::wstring& password) const;
	const std::wstring& UserName() const;
	const std::wstring& Name() const;

private:
	std::wstring m_name;
	std::wstring m_username;
	std::wstring m_password;

};

#endif // ! USER_CLASS
