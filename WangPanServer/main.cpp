#include "Server.h"

int main(int _argc, char** _args)
{
	Server server;

	server.Listen("127.0.0.1", 12345);

	return 0;
}
