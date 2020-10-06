#include "Server.h"
#include "Database.hpp"

int main(int _argc, char** _args)
{
	std::string ipaddr;
	int port;

	if(_argc != 3)
	{
		//std::cout << "args[1] is ip, args[2] is port" << std::endl;
		//return -1;
		ipaddr = "127.0.0.1";
		port = 9999;
	}
	else
	{
		ipaddr = _args[1];
		port = std::atoi(_args[2]);
	}

	db::Database db;

	db.UseDb("WangPanServer");

	try
	{
		Server server;
	
		server.Listen(ipaddr.c_str(), port);
	}
	catch(std::runtime_error& _ex)
	{
		std::cout << _ex.what() << std::endl;
	}
	
	return 0;
}
