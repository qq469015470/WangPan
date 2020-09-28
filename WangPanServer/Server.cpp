#include "Server.h"

std::array<std::string, 32> GetArgs(const std::string& _message)
{
	std::string::size_type left(0);
	std::string::size_type right(0);

	std::array<std::string, 32> result;
	size_t i(0);
	do
	{
		left = _message.find_first_not_of(' ', right);
		if(left == std::string::npos)
			break;

		right = _message.find(' ', left + 1);

		result[i++] = _message.substr(left, right - left);

	}while(right != std::string::npos && i < result.size());

	return result;
}

FinalReact::FinalReact(UserService* _userService, FileService* _fileService):
		userService(_userService),
		fileService(_fileService),
		curState(State::CORE)
{
	using TYPE = std::remove_pointer<decltype(this)>::type;
	this->cmdMap.insert(std::pair<std::string, CommandFunc>("login", std::bind(&TYPE::LoginCommand, this, std::placeholders::_1)));
	this->cmdMap.insert(std::pair<std::string, CommandFunc>("register", std::bind(&TYPE::RegisterCommand, this, std::placeholders::_1)));
	this->cmdMap.insert(std::pair<std::string, CommandFunc>("upload", std::bind(&TYPE::UploadCommand, this, std::placeholders::_1)));
}

std::vector<char> FinalReact::LoginCommand(const std::array<std::string, 32>& _args)
{
	std::string temp("login error");
	std::vector<char> result(temp.begin(), temp.end());
	auto user = this->userService->Login(_args[1], _args[2]);
	if(user.has_value())
	{
		result.clear();
		result.insert(result.begin(), user->begin(), user->end());
	}
	
	return result;
}

std::vector<char> FinalReact::RegisterCommand(const std::array<std::string, 32>& _args)
{
	std::string temp("register succecss");
	std::vector<char> result(temp.begin(), temp.end());

	this->userService->Register(_args[1], _args[2]);
	
	return result;
}

std::vector<char> FinalReact::UploadCommand(const std::array<std::string, 32>& _args)
{
	std::string temp("token failed!");
	std::vector<char> result(temp.begin(), temp.end());

	auto user = this->userService->GetUser(_args[1]);
	if(user.has_value())
	{
		result.clear();
		temp = "upload ready";
		result.clear();
		result.insert(result.begin(), temp.begin(), temp.end());

		this->uploadPath = user->name +_args[3];
		this->uploadFileName = _args[2];
		this->offset = 0;
	       	this->fileSize = std::stoll(_args[4]);
		this->fileService->ParpareFile(this->uploadFileName.c_str(), this->uploadPath.c_str(), this->fileSize);
		this->curState = State::UPLOAD;
	}

	return result;
}

void FinalReact::AcceptFileBuffer(const char* _buffer, size_t _len)
{
	this->fileService->AcceptFile((this->uploadPath + this->uploadFileName).c_str(), this->offset, _buffer, _len);
	this->offset += _len;
}

std::vector<char> FinalReact::GetRecvStr(const char* _message, size_t _len) 
{
	std::string temp;
	std::vector<char> result(temp.begin(), temp.end());	
	
	switch(this->curState)
	{
		case State::CORE:
		{
			auto args = GetArgs(std::string(_message, _len));
			try
			{


				//result = this->coreReact.GetRecvStr(_message, _len);
				//std::string str(result.begin(), result.end());
				auto iter = this->cmdMap.find(args[0]);
				if(iter != this->cmdMap.end())
				{
					result = iter->second(args);
				}
				else
				{
					temp = "command not exists";
					result.clear();
					result.insert(result.begin(), temp.begin(), temp.end());
				}
			}
			catch(std::runtime_error& _ex)
			{
				temp = args[0] + " error:" + _ex.what();
				result.clear();
				result.insert(result.begin(), temp.begin(), temp.end());
			}
			catch(std::logic_error& _ex)
			{
				temp = args[0] + " error:" + _ex.what();
				result.clear();
				result.insert(result.begin(), temp.begin(), temp.end());
			}

			break;
		}
		case State::UPLOAD:
		{
			this->AcceptFileBuffer(_message, _len);
			
			//传输完毕
			if(this->offset == this->fileSize)
			{
				this->curState = State::CORE;
				temp = "upload success";
				result.clear();
				result.insert(result.begin(), temp.begin(), temp.end());
			}
			break;
		}
		default:
		{
			throw std::logic_error("state not exists");
			break;
		}
	}
	                                                                      
	return result;
}

Server::Server():
	epfd(0),
	serverSock(0),
	listenSignal(false),
	listening(false)
{


	this->serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(this->serverSock == -1)
		throw std::runtime_error("init sock failed!");

	//bind 普遍遭遇的问题是试图绑定一个已经在使用的端口。
	//该陷阱是也许没有活动的套接字存在，但仍然禁止绑定端口（bind 返回 EADDRINUSE），
	//它由 TCP 套接字状态 TIME_WAIT 引起。该状态在套接字关闭后约保留 2 到 4 分钟。
	//在 TIME_WAIT 状态退出之后，套接字被删除，该地址才能被重新绑定而不出问题。
	//等待 TIME_WAIT 结束可能是令人恼火的一件事，特别是如果您正在开发一个套接字服务器，就需要停止服务器来做一些改动，然后重启。
	//幸运的是，有方法可以避开 TIME_WAIT 状态。可以给套接字应用 SO_REUSEADDR 套接字选项，以便端口可以马上重用。
	
	const int opt = 1;
	if(setsockopt(this->serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
	{
		throw std::runtime_error("setsockopt failed");
	}
	
	this->epfd = epoll_create(this->maxClient);
	if(this->epfd == -1)
		throw std::runtime_error("init epfd failed!");
}

Server::~Server()
{
	close(this->epfd);
	close(this->serverSock);
}

void Server::AcceptClient()
{
	sockaddr_in clntAddr = {};
	socklen_t size = sizeof(clntAddr);

	const int connfd = accept(this->serverSock, reinterpret_cast<sockaddr*>(&clntAddr), &size);
	if(connfd == -1)
		throw std::runtime_error("accept failed!");

	this->reactMap.insert(std::pair<int, std::unique_ptr<IRecvReact>>(connfd, std::make_unique<FinalReact>(&this->userService, &this->fileService)));

	epoll_event event;

	event.data.fd = connfd;
	event.events = EPOLLIN;

	epoll_ctl(this->epfd, EPOLL_CTL_ADD, connfd, &event);
	std::cout << "accept client addr:" << inet_ntoa(clntAddr.sin_addr) << std::endl;
}

void Server::RecvClient(epoll_event& _ev)
{
	const int& clntSock = _ev.data.fd;
	char buffer[1024];
	memset(buffer, 0 , sizeof(buffer));

	const int recvLen = recv(clntSock, buffer, sizeof(buffer), 0);
	if(recvLen <= 0)
	{
		std::cout << "client close" << std::endl;
		this->CloseClient(_ev);

	}
	else
	{
		std::cout << "recv:" << std::string(buffer, recvLen) << std::endl;

		const std::vector<char> reply = this->reactMap.at(clntSock)->GetRecvStr(buffer, recvLen);
		if(reply.size() > 0)
			send(clntSock, reply.data(), reply.size(), 0);
	}
}

void Server::CloseClient(epoll_event& _ev)
{
	const int& clntSock = _ev.data.fd;
	this->reactMap.erase(clntSock);
	epoll_ctl(this->epfd, EPOLL_CTL_DEL, clntSock, &_ev);
	
	close(clntSock);
}

void Server::ListenProc(sockaddr_in _sockAddr)
{
	//初始化socket
	if(bind(this->serverSock, reinterpret_cast<sockaddr*>(&_sockAddr), sizeof(_sockAddr)) == -1)
	{
		perror("bind");
		throw std::runtime_error("bind socket failed!");
	}

	if(listen(this->serverSock, this->maxClient))
		throw std::runtime_error("listen failed!");

	if(this->serverSock == -1)
		throw std::runtime_error("create server socket faile!");

	//初始化epoll
	epoll_event events[this->maxClient];

	epoll_event ev;
	ev.data.fd = this->serverSock; 
	ev.events = EPOLLIN;
	if(epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->serverSock, &ev) == -1)
		throw std::runtime_error("epoll ctl failed!");

	this->listening = true;
	while(this->listenSignal == true)
	{
		const int nfds = epoll_wait(this->epfd, events, this->maxClient, -1);
		if(this->listenSignal == false)
			break;

		if(nfds == -1)
		{
			std::cout << "epoll wait failed!" << std::endl;	
		}

		for(int i = 0; i < nfds; i++)
		{
			if(events[i].data.fd == this->serverSock)
			{
				try
				{
					this->AcceptClient();	
				}
				catch(std::runtime_error& _ex)
				{
					std::cout << "accept error:" << _ex.what() << std::endl;
				}
			}
			else
			{
				try
				{
					this->RecvClient(events[i]);
				}
				catch(std::runtime_error& _ex)
				{
					std::cout << "recv error:" << _ex.what() << std::endl;
				}
			}
		}		
	}

	this->listening = false;
	this->listenSignal = false;
	std::cout << "server close" << std::endl;
}

Server& Server::Listen(const char* _ipaddress, int _port)
{
	if(this->listenSignal == true)
		throw std::logic_error("server is listening!");

	this->listenSignal = true;

	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(_ipaddress);
	serverAddr.sin_port = htons(_port);

	std::cout << "server listening... ip:" << _ipaddress << " port:" << ntohs(serverAddr.sin_port) << std::endl;
	this->ListenProc(serverAddr);

	return *this;
}

Server& Server::Stop()
{
	if(this->listenSignal == false)
		throw std::logic_error("server not listening!");	

	this->listenSignal = false;

	const int sfd = eventfd(1, 0); 
	if(sfd == -1)
		throw std::runtime_error("signalfd failed");
	
	epoll_event event;
  	event.data.fd = sfd;
  	event.events = EPOLLIN;
  	if(epoll_ctl(this->epfd, EPOLL_CTL_ADD, sfd, &event) == -1)
		throw std::runtime_error("epoll ctl failed");

	while(this->listening);

	if(close(sfd) != 0)
		throw std::runtime_error("close eventfd failed!");


	return *this;
}

bool Server::IsListening()
{
	return this->listening;
}
