#pragma once

#include <iostream>
#include <stdexcept>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

class Server
{
private:
	static constexpr int maxClient = 20;
	int epfd;
	int serverSock;

	void AcceptClient()
	{
		sockaddr_in clntAddr = {};
		socklen_t size = sizeof(clntAddr);

		const int connfd = accept(this->serverSock, reinterpret_cast<sockaddr*>(&clntAddr), &size);
		if(connfd == -1)
			throw std::runtime_error("accept failed!");

		epoll_event event;

		event.data.fd = connfd;
		event.events = EPOLLIN;

		epoll_ctl(this->epfd, EPOLL_CTL_ADD, connfd, &event);
		std::cout << "accept client addr:" << inet_ntoa(clntAddr.sin_addr) << std::endl;
	}

	void RecvClient(epoll_event& _ev)
	{
		const int& clntSock = _ev.data.fd;
		char buffer[1024];
		memset(buffer, 0 , sizeof(buffer));

		const int recvLen = recv(clntSock, buffer, sizeof(buffer), 0);
		if(recvLen <= 0)
		{
			std::cout << "client close" << std::endl;
			epoll_ctl(this->epfd, EPOLL_CTL_DEL, clntSock, &_ev);
			close(clntSock);
		}
		else
		{
			std::cout << "recv:" << buffer << std::endl;
		}
	}

	//监听
	void ListenProc(sockaddr_in _sockAddr)
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
		this->epfd = epoll_create(this->maxClient);
		if(epfd == -1)
			throw std::runtime_error("create epoll failed!");

		epoll_event ev;
		ev.data.fd = this->serverSock; 
		ev.events = EPOLLIN;
		if(epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->serverSock, &ev) == -1)
			throw std::runtime_error("epoll ctl failed!");

		while(true)
		{
			const int nfds = epoll_wait(this->epfd, events, this->maxClient, -1);
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
					catch(std::runtime_error _ex)
					{
						std::cout << _ex.what() << std::endl;
					}
				}
				else
				{
					this->RecvClient(events[i]);
				}
			}		
		}
	}

public:
	Server():
		serverSock(0),
		epfd(0)
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

	~Server()
	{
		close(this->epfd);
		close(this->serverSock);
	}

	void Listen(const char* _ipaddress, int _port)
	{
		sockaddr_in serverAddr{};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_addr.s_addr = inet_addr(_ipaddress);
		serverAddr.sin_port = htons(_port);

		std::cout << "server listening... ip:" << _ipaddress << " port:" << ntohs(serverAddr.sin_port) << std::endl;
		this->ListenProc(serverAddr);
	}
};
