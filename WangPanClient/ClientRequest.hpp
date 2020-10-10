#pragma once

#include <QTcpSocket>
#include <QHostAddress>

#include <string>
#include <fstream>
#include <thread>

class ClientRequest
{
private:
	QHostAddress host;
	int port;
	
	inline void ConnectToHost(QTcpSocket* _qSock)
	{
		_qSock->connectToHost(this->host, this->port);
		if(_qSock->waitForConnected() == false)
		{
			throw std::runtime_error("连接服务器失败");
		}
	}

public:
	inline ClientRequest():
		host(QHostAddress("127.0.0.1")),
		port(9999)
	{

	}

	//返回token
	inline std::string Login(std::string _username, std::string _password)
	{
		QTcpSocket qSock;
	
		this->ConnectToHost(&qSock);
	
		std::string temp("login ");
		temp += _username;
		temp += " ";
		temp += _password;
	
		if(qSock.write(temp.data(), temp.size()) == -1)
		{
			throw std::runtime_error("发送信息失败");
		}
	
		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();
	
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		const qint64 recvLen = qSock.read(buffer, sizeof(buffer));
		if(recvLen <= 0)
		{
			throw std::runtime_error("接收信息失败");
		}
	
		temp = std::string(buffer, recvLen);
			
		if(temp == "login error")
		{
			throw std::runtime_error("登录失败");	
		}
	
		return temp;
	}

	inline void Register(std::string _username, std::string _password)
	{
		QTcpSocket qSock;

		this->ConnectToHost(&qSock);

		std::string temp("register ");
		temp += _username;
		temp += " ";
		temp += _password;
	
		if(qSock.write(temp.data(), temp.size()) == -1)
		{
			throw std::runtime_error("发送信息失败");
		}
	
		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();
		
		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		const qint64 recvLen = qSock.read(buffer, sizeof(buffer));
		if(recvLen <= 0)
		{
			throw std::runtime_error("接收信息失败");
		}


		temp = std::string(buffer, recvLen);
		if(temp != "register succecss")
			throw std::runtime_error(temp);
	}

	inline std::vector<std::string> GetFilesList(const std::string& _token, const std::string& _location)
	{
		QTcpSocket qSock;
	
		this->ConnectToHost(&qSock);
	
		std::string cmd("dir ");
		cmd += _token;
		cmd += ' ';
		cmd += _location;
	
		if(qSock.write(cmd.data(), cmd.size()) == -1)
		{
			throw std::runtime_error("发送信息失败");
		}
	
		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();
	
		char buffer[1024];
		qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
		if(recvLen <= 0)
		{
			throw std::runtime_error("接收信息失败");
		}
	
		std::string temp(buffer, recvLen);
		std::string::size_type left(0);
		std::string::size_type right(temp.find(' '));
		size_t fileCount(std::stoll(temp.substr(left, right - left)));
	
		if(right != std::string::npos)
			temp = temp.substr(right + 1);

		std::vector<std::string> result;
		while(fileCount > 0 && right != std::string::npos)	
		{
			left = 0;
			right = temp.find(' ', left);
	
			std::string filename(temp.substr(left, right - left));
			temp = temp.substr(right + 1);
			
			result.push_back(filename);
	
			fileCount--;
		}

		return result;
	}

	inline void CreateDirectory(std::string _token, std::string _path)
	{
		QTcpSocket qSock;

		this->ConnectToHost(&qSock);

		std::string cmd("createdir ");
		cmd += _token;
		cmd += ' ';
		cmd += _path;

		if(qSock.write(cmd.data(), cmd.size()) == -1)
		{
			throw std::runtime_error("发送信息失败");
		}
	
		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();
	
		char buffer[1024];
		qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
		if(recvLen <= 0)
		{
			throw std::runtime_error("接收信息失败");
		}

		std::string temp(buffer, recvLen);
		if(temp != "createdir success")
			throw std::runtime_error(temp);
	}

	//阻塞上传文件，每次上传数据都会调用回调函数
	inline void UploadFile(const std::string& _token, const std::string& _filename, const char* _remoteDir, std::function<void(float)> _progressCallback)
	{
		std::string::size_type pos(_filename.find_last_of('/'));
		if(pos != std::string::npos)
			pos += 1;

		std::string path(_filename.substr(0, pos));
		std::string name(_filename.substr(pos));

		QTcpSocket qSock;

		this->ConnectToHost(&qSock);

		std::ifstream file(path + name, std::ios::in| std::ios::binary);

		if(!file.is_open())
			throw std::runtime_error("打开文件失败");

		file.seekg(0, std::ios::end);
		const size_t fileSize = file.tellg();
		file.seekg(0, std::ios::beg);

		std::string cmd("upload ");
		cmd += _token;
		cmd += ' ';
		cmd += name;
		cmd += ' ';
		cmd += _remoteDir;
		cmd += ' ';
		cmd += std::to_string(fileSize);	

		if(qSock.waitForConnected() == false)
			throw std::runtime_error("连接服务器失败");

		if(qSock.write(cmd.data(), cmd.size()) == -1)
			throw std::runtime_error("发送信息失败");

		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();

		char buffer[1024];
		memset(buffer, 0, sizeof(buffer));
		qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
		if(recvLen <= 0)
			throw std::runtime_error("接收信息失败");

		std::string temp(buffer, recvLen);
		if(temp != "upload ready")
			throw std::runtime_error(temp);

		size_t hadRead(0);

		while(hadRead < fileSize)
		{
			const size_t unReadSize = fileSize - hadRead;
			const size_t readSize = std::min(sizeof(buffer), unReadSize);
				
			memset(buffer, 0, sizeof(buffer));
			file.read(buffer, readSize);
			hadRead += readSize; 
			qSock.write(buffer, readSize);
			qSock.waitForBytesWritten();
			_progressCallback(static_cast<float>(hadRead) / fileSize);

			//模拟网络速度
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		qSock.waitForReadyRead();
		recvLen = qSock.read(buffer, sizeof(buffer));	
		temp = std::string(buffer, recvLen);
		if(temp != "upload success")
		{
			throw std::runtime_error("上传异常");
		}

		file.close();
	}

	//阻塞下载文件
	//第三个参数为回调函数
	//回调函数参数按顺序分别是(字节，字节大小, 文件总大小)
	inline void DownloadFile(const std::string _token, const std::string& _downloadPath, const std::string& _savePath, std::function<void(char*, size_t, size_t)> _callback)
	{
		QTcpSocket qSock;

		this->ConnectToHost(&qSock);

		std::string cmd("download ");
		cmd += _token;
		cmd += ' ';
		cmd += _downloadPath;

		if(qSock.write(cmd.data(), cmd.size()) == -1) throw std::runtime_error("发送信息失败");

		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();

		char buffer[4096];
		qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
		if(recvLen <= 0)
			throw std::runtime_error("接收信息失败");

		std::string temp(buffer, recvLen);
		std::string::size_type pos(temp.find("download ready"));
		if(pos == std::string::npos)
			throw std::runtime_error(temp);
		else
			pos += 14;

		const size_t fileSize = std::stoll(temp.substr(pos));

		std::ofstream file(_savePath, std::ios::out | std::ios::binary);

		if(!file.is_open())
			throw std::runtime_error("打开文件失败");

		size_t hadRead(0);
		while(hadRead < fileSize)
		{
			qSock.write("c", 1);
			qSock.waitForBytesWritten();
			qSock.waitForReadyRead();

			//注意接收大小必须要与服务器发送的大小一致
			recvLen = qSock.read(buffer, sizeof(buffer));
			if(recvLen <= 0)
				throw std::runtime_error("下载文件失败");

			file.write(buffer, recvLen);
			hadRead+= recvLen;

			_callback(buffer, recvLen, fileSize);
		}
		
		file.close();
	}

	inline void RemoveFile(const std::string& _token, const std::string& _path)
	{
		QTcpSocket qSock;
		this->ConnectToHost(&qSock);
		
		std::string cmd("rmfile ");
		cmd += _token;
		cmd += ' ';
		cmd += _path;

		if(qSock.write(cmd.data(), cmd.size()) == -1)
		{
			throw std::runtime_error("发送信息失败");
		}
	
		qSock.waitForBytesWritten();
		qSock.waitForReadyRead();
	
		char buffer[1024];
		qint64 recvLen(qSock.read(buffer, sizeof(buffer)));
		if(recvLen <= 0)
		{
			throw std::runtime_error("接收信息失败");
		}

		std::string temp(buffer, recvLen);
		if(temp != "rmfile success")
			throw std::runtime_error(temp);
	}
};
