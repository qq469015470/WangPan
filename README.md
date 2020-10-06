# 项目描述
* 网盘系统,包含客户端及服务器。 <br>
* 数据库使用mongodb，安装连接:https://www.mongodb.com/try/download/community。 <br>
* 服务器使用epoll模型，接收客户端信息，接收的客户端文件存放在UserFile文件夹，例如账号admin存放的文件目录位于文件夹/UserFile/admin/下。 <br>
* 客户端使用QT框架，使用QTcpSocket与服务器连接。 <br>
* 目录下的WangPanClient为客户端代码文件，WangPanServer为服务器代码文件，WangPanServer下的unittest为单元测试代码文件。

# CMake版本需求
该项目使用CMake管理,版本需求为 **3.5** 以上

# 引用库
### QT
https://www.qt.io/download<br>
用于客户端UI的框架。
### mongo-cxx-driver <br>
http://mongocxx.org/mongocxx-v3/installation/ <br>
用于与mongodb数据库交互。
### gtest
https://github.com/google/googletest <br>
用于服务器单元测试的卡框架。


# 编译运行
### 客户端编译
````shell
cd WangPanClient
mkdir build
cd build
cmake .. #-DCMAKE_PREFIX_PATH=填写QT库路径
make
./WangPanClient #运行
````

### 服务器编译
````shell
cd WangPanServer
mkdir build
cd build
cmake ..
make
./WangPanServer #默认使用本地ip及9999端口号,可提供运行参数修改 参数1为ip地址 参数2为端口号 例如: ./WangPanServer 127.0.0.1 9999
````

### 服务器单元测试
````shell
cd WangPanServer
cd unittest
mkdir build
cd build
cmake ..
make
cd bin
sh unittest.sh #该脚本会创建测试用例数据并执行UnitTest程序文件
````
