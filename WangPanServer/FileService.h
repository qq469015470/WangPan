#pragma once

#include <fstream>
#include <stdexcept>
#include <vector>

class FileService
{
public:
	static inline const char* storeBasePath = "./UserFile/";
	//上传中文件的后缀
	static inline const char* fileSuffix = "upload"; 

	FileService();

	//初始化要上传的文件
	void ParpareFile(const char* _filename, const char* _storePath, size_t _size);

	//检测是否有处于上传中的文件
	//不需要加上.upload
	bool HadParpareFile(const char* _path);

	//接收上传文件的字节
	void AcceptFile(const char* _path, size_t _offset, const char* _bytes, size_t _len);
	
	void CreateDirectory(const char* _path);

	void RemoveFile(const char* _path);

	std::vector<std::string> DirFiles(const char* _path);
};
