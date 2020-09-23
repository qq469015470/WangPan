#include <fstream>
#include <stdexcept>

class FileService
{
private:
	const char* storeBasePath = "./UserFile/";
	//上传中文件的后缀
	const char* fileSuffix = "upload"; 

public:
	FileService()
	{

	}

	//初始化要上传的文件
	void ParpareFile(const char* _filename, const char* _storePath, size_t _size)
	{
		std::string writePath(storeBasePath);
		writePath += _storePath;
		writePath += _filename;

		//检查文件是否存在
		std::ifstream check(writePath, std::ios::in);
		if(check.is_open() || this->HadParpareFile(writePath.c_str()))
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

	//检测是否有处于上传中的文件
	//不需要加上.upload
	bool HadParpareFile(const char* _path)
	{
		std::string path(_path);
		path += '.';
		path += this->fileSuffix;
		std::ifstream check(path, std::ios::in);

		return check.is_open();
	}

	//接收上传文件的字节
	void AcceptFile(const char* _path, size_t _offset, const char* _bytes, size_t _len)
	{
		std::ofstream file(_path, std::ios::out| std::ios::binary);

		if(!file.is_open())
		{

		}

		file.close();
	}
};
