#include "../Server.h"

#include <thread>
#include <chrono>

class ServerTest: public testing::Test
{
private:
	Server server;
	std::thread serverProc;

	static void Listen(Server& _server)
	{
		_server.Listen("127.0.0.1", 12345);
	}

protected:
	virtual void SetUp()
	{
		this->serverProc = std::thread(Listen, std::ref(this->server));

		while(!this->server.IsListening());
		//std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}

	virtual void TearDown()
	{
		this->server.Stop();
		this->serverProc.join();				
	}
};

int Connect(const int& _sockfd, const char* _ipaddress, int _port)
{
	timeval timeout={1, 0};//1.0s
    	setsockopt(_sockfd,SOL_SOCKET,SO_SNDTIMEO,(const char*)&timeout,sizeof(timeout));
    	setsockopt(_sockfd,SOL_SOCKET,SO_RCVTIMEO,(const char*)&timeout,sizeof(timeout));

	sockaddr_in serverAddr{};	
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(_ipaddress);
	serverAddr.sin_port = htons(_port);

	return connect(_sockfd, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
}

//测试登录
TEST_F(ServerTest, ClientLogin)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);
	
	char buffer[1024] = "login testA 123123"; 
	send(sockfd, buffer, strlen(buffer), 0);
	memset(buffer, 0, sizeof(buffer));
       	const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
	ASSERT_TRUE(recvLen > 0);

	std::string recvStr(buffer, recvLen);
	EXPECT_NE(recvStr, "login error");
	
	close(sockfd);

	UserService userService;

	auto user = userService.GetUser(recvStr);
	EXPECT_TRUE(user.has_value());
}

//上传命令
TEST_F(ServerTest, UploadTokenFailed)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);
	
	char buffer[1024] = "upload 111111111111111111111111"; 
	ASSERT_TRUE(send(sockfd, buffer, strlen(buffer), 0) != -1);
	memset(buffer, 0, sizeof(buffer));
       	const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
	ASSERT_TRUE(recvLen > 0);

	std::string recvStr(buffer, recvLen);
	EXPECT_EQ(recvStr, "token failed!");
	
	close(sockfd);
}


TEST_F(ServerTest, UploadSuccess)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);

	{
		char buffer[1024] = "upload 5349b4ddd2781d08c09890f3 unittestfile.txt / 3";
		ASSERT_TRUE(send(sockfd, buffer, strlen(buffer), 0) != -1);
		memset(buffer, 0, sizeof(buffer));
       		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "upload ready");
	}

	{
		char buffer[1024] = "abc";
		ASSERT_TRUE(send(sockfd, buffer, 3, 0) != -1);
		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "upload success");
	}
	
	close(sockfd);
}
