#include <gtest/gtest.h>
#include <stdio.h>
#include <filesystem>

#include "../FileService.h"

TEST(FileServiceTest, ParpareExistsFile)
{
	FileService fileService;

	EXPECT_THROW(fileService.ParpareFile("test.txt", "qq469015470/", 111), std::logic_error);
	EXPECT_THROW(fileService.ParpareFile("uploading.txt", "qq469015470/", 111), std::logic_error);

	std::ifstream check;

	check.open("UserFile/qq469015470/uploading.txt.upload", std::ios::in);
	EXPECT_FALSE(!check.is_open());
	check.close();
}

TEST(FileServiceTest, ParpareFile)
{
	FileService fileService;

	EXPECT_NO_THROW(fileService.ParpareFile("aaa.txt", "qq469015470/", 3));

	std::ifstream check;

	check.open("UserFile/qq469015470/aaa.txt.upload", std::ios::in);
	ASSERT_TRUE(check.is_open());
	
    	check.seekg(0, std::ios::end);
    	size_t fileLen = check.tellg();
	check.seekg(0, std::ios::beg);

	EXPECT_EQ(fileLen, 3);

	check.close();
}

TEST(FileServiceTest, AcceptFile)
{
	FileService fileService;

	std::string temp("abc");

	EXPECT_NO_THROW(fileService.AcceptFile("qq469015470/aaa.txt", 0, temp.data(), 3));

	std::ifstream check("UserFile/qq469015470/aaa.txt", std::ios::in| std::ios::binary);

	ASSERT_TRUE(check.is_open());
	 //获取文件大小
    	check.seekg(0, std::ios::end);
    	size_t fileLen = check.tellg();
	check.seekg(0, std::ios::beg);

	ASSERT_EQ(fileLen, 3);
	char buffer[3];
	check.read(buffer, 3);

	EXPECT_EQ(std::string(buffer, 3), temp);

	check.close();
}

TEST(FileServiceTest, CreateDirectory)
{
	FileService fileService;

	fileService.CreateDirectory("/UnitTestDir");	

	EXPECT_TRUE(std::filesystem::exists("UserFile/UnitTestDir"));
}

TEST(FileServiceTest, CreateExistsDirectory)
{

	FileService fileService;

	EXPECT_THROW(fileService.CreateDirectory("/qq469015470"), std::runtime_error);
}
