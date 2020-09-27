#include "Server.h"
#include "Database.hpp"

int main(int _argc, char** _args)
{
	if(_argc != 3)
	{
		std::cout << "args[1] is ip, args[2] is port" << std::endl;
		return -1;
	}

	db::Database db;

	db.UseDb("WangPanServer");

	try
	{
		Server server;
	
		server.Listen(_args[1], std::atoi(_args[2]));
	}
	catch(std::runtime_error& _ex)
	{
		std::cout << _ex.what() << std::endl;
	}
	
	return 0;
}
