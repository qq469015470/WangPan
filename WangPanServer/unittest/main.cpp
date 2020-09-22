#include <gtest/gtest.h>
#include "TestUserService.cpp"

int main(int _argc, char** _args)
{
	db::Database db;

	db.UseDb("UnitTestDB");

	::testing::InitGoogleTest(&_argc, _args);

	return RUN_ALL_TESTS();	
}
