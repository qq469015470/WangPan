#pragma once

#include "UserService.h"
#include "FileService.h"

#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

//接收信息反馈接口
class IRecvReact
{
public:
	virtual ~IRecvReact() = default;

	virtual std::vector<char> GetRecvStr(const char* _message, size_t _len) = 0;
};

class FinalReact: virtual public IRecvReact
{
private:
	using CommandFunc = std::function<std::vector<char>(const std::array<std::string, 32>&)>;

	enum class State
	{
		CORE,
		UPLOAD
	};

	UserService* userService;
	FileService* fileService;

	State curState;
	std::string uploadPath;
	std::string uploadFileName;
	size_t offset;
	size_t fileSize;
	std::unordered_map<std::string, CommandFunc> cmdMap;

	std::vector<char> LoginCommand(const std::array<std::string, 32>& _args);
	std::vector<char> RegisterCommand(const std::array<std::string, 32>& _args);
	std::vector<char> UploadCommand(const std::array<std::string, 32>& _args);
	std::vector<char> DirCommand(const std::array<std::string, 32>& _args);

	void AcceptFileBuffer(const char* _buffer, size_t _len);

public:
	FinalReact(UserService* _userService, FileService* _fileService);

	virtual	std::vector<char> GetRecvStr(const char* _message, size_t _len) override;
};


class Server
{
private:
	static constexpr int maxClient = 20;
	int epfd;
	int serverSock;
	bool listenSignal;
	bool listening;

	std::unordered_map<int, std::unique_ptr<IRecvReact>> reactMap;
	
	UserService userService;
	FileService fileService;

	void AcceptClient();

	void RecvClient(epoll_event& _ev);

	void CloseClient(epoll_event& _ev);

	//监听
	void ListenProc(sockaddr_in _sockAddr);
public:
	Server();

	~Server();

	Server& Listen(const char* _ipaddress, int _port);

	Server& Stop();

	bool IsListening();
};
