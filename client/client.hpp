#include"../TaskSocket.hpp"
#include<iostream>
#include<chrono>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<termios.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<nlohmann/json.hpp>
#include<hiredis/hiredis.h>
#define SERVERPORT 8080 //默认端口号
#define RECV -1
#define SIGNUP 1
#define LOGIN 2
#define LOGOUT 3
#define FRIENDLIST 4
#define ADDFRIEND 5
#define DELETEFRIEND 6
#define AGREEADDFRIEND 7
#define REFUSEADDFRIEND 8
#define BLOCKFRIEND 9
#define RESTOREFRIEND 10
#define SENDFILEGROUP 11
#define RECVFILEGROUP 12
#define SENDFILE 13
#define RECVFILE 14
using namespace std;
using json=nlohmann::json;
extern int client_fd;
extern TaskSocket asocket;
extern sockaddr_in server_addr2;
