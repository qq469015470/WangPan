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

//测试客户端下载一半断开
TEST_F(ServerTest, UploadHalf)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);
	
	{
		char buffer[1024] = "upload 5349b4ddd2781d08c09890f3 uploadhalf / 1024";
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);
		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "upload ready");
	
		ASSERT_NE(send(sockfd, buffer, 512, 0), -1);
	}
	close(sockfd);

	//等待服务器删除文件
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	std::ifstream check("UserFile/testB/uploadhalf.upload", std::ios::in | std::ios::binary);

	EXPECT_FALSE(check.is_open());

	check.close();	
}

TEST_F(ServerTest, UploadSuccess)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);

	{
		char buffer[1024] = "upload 5349b4ddd2781d08c09890f3 unittestfile.txt / 3";
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);
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

	std::ifstream check("UserFile/testB/unittestfile.txt", std::ios::in| std::ios::binary);

	EXPECT_TRUE(check.is_open());
	
	close(sockfd);
}

TEST_F(ServerTest, UploadTwiceSuccess)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);

	{
		char buffer[1024] = "upload 5349b4ddd2781d08c09890f3 unittestfile2.txt / 3000";
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);
		memset(buffer, 0, sizeof(buffer));
       		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "upload ready");
	}

	{
		char buffer[3000];
		ASSERT_TRUE(send(sockfd, buffer, 1000, 0) != -1);
		ASSERT_TRUE(send(sockfd, buffer, 1000, 0) != -1);
		ASSERT_TRUE(send(sockfd, buffer, 1000, 0) != -1);

		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "upload success");
	}

	std::ifstream check("UserFile/testB/unittestfile2.txt", std::ios::in| std::ios::binary);

	EXPECT_TRUE(check.is_open());
	
	close(sockfd);
}

//显示目录
TEST_F(ServerTest, Dir)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);

	{
		char buffer[1024] = "dir 5f719c3ceca2144f4b36b200 /";
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);

		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "4 *removedir *wenjianjia abc.txt hello.txt");
	}

	close(sockfd);
}

TEST_F(ServerTest, CreateDir)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);

	{
		char buffer[1024] = "createdir 5f719c3ceca2144f4b36b200 /TestCreateDir"; 
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);

		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "createdir success");
	}
}

TEST_F(ServerTest, RemoveFile)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);

	{
		char buffer[1024] = "rmfile 5f719c3ceca2144f4b36b200 /removedir";
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);

		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "rmfile success");
	}

}

TEST_F(ServerTest, DownloadFile)
{
	const int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	EXPECT_EQ(Connect(sockfd, "127.0.0.1", 12345), 0);
	
	{
		char buffer[1024] = "download 5349b4ddd2781d08c09890f3 /downfile.txt";
		ASSERT_NE(send(sockfd, buffer, strlen(buffer), 0), -1);

		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);	
		ASSERT_TRUE(recvLen > 0);

		std::string recvStr(buffer, recvLen);
		EXPECT_EQ(recvStr, "download ready");
	}

	{
		char buffer[1024] = "c";
		ASSERT_NE(send(sockfd, buffer, 1, 0), -1);

		memset(buffer, 0, sizeof(buffer));
		const int recvLen = recv(sockfd, buffer, sizeof(buffer), 0);
		ASSERT_TRUE(recvLen > 0);

		//打开文件比较是否一致
		std::ifstream check("UserFile/testB/downfile.txt", std::ios::in| std::ios::binary);
		ASSERT_TRUE(check.is_open());
		check.seekg(0, std::ios::end);
		const int fileSize = check.tellg();
		check.seekg(0, std::ios::beg);
		
		ASSERT_EQ(recvLen, fileSize);

		char fileBuffer[1024];
		check.read(fileBuffer, sizeof(fileBuffer));

		for(int i = 0; i < fileSize; i++)
			ASSERT_EQ(buffer[i], fileBuffer[i]);

		check.close();
	}
}
