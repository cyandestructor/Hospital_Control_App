#include "User.h"

bool User::CheckUser(const std::wstring& username, const std::wstring& password) const {
	
	return m_username == username && m_password == password;

}

const std::wstring& User::UserName() const {

	return m_username;

}

const std::wstring& User::Name() const {
	
	return m_name;

}

std::wostream& operator<<(std::wostream& os, const User& user) {
	
	os << user.m_name << std::endl
		<< user.m_username << std::endl
		<< user.m_password << std::endl;

	return os;

}

std::wistream& operator>>(std::wistream& is, User& user) {

	std::getline(is, user.m_name);
	std::getline(is, user.m_username);
	std::getline(is, user.m_password);

	return is;

}