#pragma once

#include "Database.hpp"

#include <string>

struct User
{
	std::string name;
	std::string password;
};

class UserService
{
private:
	db::Table table;

public:
	UserService();

	User Register(std::string _name, std::string _password);

	std::optional<std::string> Login(std::string _name, std::string _password);

	std::optional<User> GetUser(std::string _token);
};
