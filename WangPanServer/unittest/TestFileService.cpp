#include <gtest/gtest.h>
#include <stdio.h>

#include "../FileService.h"

TEST(FileService, ParpareExistsFile)
{
	FileService fileService;

	EXPECT_THROW(fileService.ParpareFile("test.txt", "qq469015470/", 111), std::logic_error);
	EXPECT_THROW(fileService.ParpareFile("uploading.txt", "qq469015470/", 111), std::logic_error);

	std::ifstream check;

	check.open("UserFile/qq469015470/uploading.txt.upload", std::ios::in);
	EXPECT_FALSE(!check.is_open());
	check.close();
}

TEST(FileService, ParpareFile)
{
	FileService fileService;

	EXPECT_NO_THROW(fileService.ParpareFile("aaa.txt", "qq469015470/", 123456));

	std::ifstream check;

	check.open("UserFile/qq469015470/aaa.txt.upload", std::ios::in);
	EXPECT_TRUE(check.is_open());
	check.close();
}
