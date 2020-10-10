#include "FileService.h"

#include <array>
#include <filesystem>
#include <algorithm>

FileService::FileService()
{

}

void FileService::ParpareFile(const char* _filename, const char* _storePath, size_t _size)
{
	std::string writePath(this->storeBasePath);
	writePath += _storePath;
	writePath += _filename;

	//检查文件是否存在
	std::ifstream check(writePath, std::ios::in);
	if(check.is_open() || this->HadParpareFile((std::string(_storePath) + _filename).c_str()))
	{
		throw std::logic_error("file exists");
	}
	check.close();


	//创建并写入基本信息
	writePath += '.';
	writePath += this->fileSuffix;

	std::ofstream file(writePath, std::ios::out| std::ios::binary);
	if(!file.is_open())
	{
		throw std::runtime_error("can not open file");
	}

	while(_size > 0)
	{
		std::array<char, 1024> temp;
		if(_size >= 1024)
		{
			file.write(temp.data(), temp.size());
			_size -= 1024;
		}
		else
		{
			file.write(temp.data(), _size);
			_size -= _size;
		}

	}
	
	file.close();	
}

bool FileService::HadParpareFile(const char* _path)
{
	std::string path(this->storeBasePath);
	path += _path;
	path += '.';
	path += this->fileSuffix;
	std::ifstream check(path, std::ios::in);

	return check.is_open();
}

void FileService::AcceptFile(const char* _path, size_t _offset, const char* _bytes, size_t _len)
{
	std::string path(this->storeBasePath);
	path += _path;

	std::string fileName(path.substr(path.find_last_of('/') + 1));
	std::string uploadName(fileName + '.' + this->fileSuffix);

	//去掉文件名
	path = path.substr(0, path.find_last_of('/')) + '/';

	std::fstream file(path + uploadName, std::ios::in | std::ios::out| std::ios::binary);

	if(file.is_open())
	{
    		file.seekg(0, std::ios::end);
    		size_t fileLen = file.tellg();

		file.seekp(_offset, std::ios::beg);
		file.write(_bytes, _len);

		if(fileLen == _offset + _len)
		{
			std::string cmd("mv ");
			cmd += path + uploadName;
			cmd += " ";
			cmd += path + fileName;
			system(cmd.c_str());
		}
		file.close();
	}
	else
	{
		throw std::runtime_error("file not exists");
	}

}

void FileService::CreateDirectory(const char* _path)
{
	if(std::filesystem::create_directory(this->storeBasePath + std::string(_path)) == false)
		throw std::runtime_error("create directory failed!");
}

void FileService::RemoveFile(const char* _path)
{
	if(std::filesystem::remove(this->storeBasePath + std::string(_path)) == false)
		throw std::runtime_error("remove file failed!");
}

std::vector<std::string> FileService::DirFiles(const char* _path)
{
	std::string path(this->storeBasePath);
	path += _path;

	std::vector<std::string> files;

	for(const auto& item: std::filesystem::directory_iterator(path))
	{
		if(item.path().extension().string() == std::string(".") + this->fileSuffix)
			continue;

		files.push_back(item.path().filename().string());
		if(item.is_directory())
			files.back() = "*" + files.back();
	}

	std::sort(files.begin(), files.end());

	return files;
}

size_t FileService::GetFileSize(const char* _path)
{
	std::ifstream file(std::string(this->storeBasePath) + _path, std::ios::in| std::ios::binary);

	if(!file.is_open())
		throw std::logic_error("path not exists");

    	file.seekg(0, std::ios::end);
    	const size_t fileLen = file.tellg();

	file.close();

	return fileLen;
}

std::vector<char> FileService::GetFileBuffer(const char* _path, size_t _offset, size_t _size)
{
	std::ifstream file(std::string(this->storeBasePath) + _path, std::ios::in | std::ios::binary);

	if(!file.is_open())
		throw std::logic_error("path not exists");

	std::vector<char> result;
	result.resize(_size);

    	file.seekg(_offset, std::ios::beg);

	file.read(result.data(), result.size());

	file.close();	

	return result;
}
