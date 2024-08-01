#include"../TaskSocket.hpp"
#include"../Redis.hpp"
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
#define ADD_FRIEND 5
#define DELETE_FRIEND 6
#define AGREE_ADDFRIEND 7
#define REFUSE_ADDFRIEND 8
#define BLOCK_FRIEND 9
#define RESTORE_FRIEND 10
#define SENDFILE_GROUP 11
#define RECVFILE_GROUP 12
#define SENDFILE 13
#define RECVFILE 14
#define QUESTION_GET 15
#define PASSWORD_FIND 16
#define PASSWORD_GET 17
using namespace std;
using json=nlohmann::json;
extern int client_fd;
extern TaskSocket asocket;
extern sockaddr_in server_addr2;

void pass_find();
void sign_up();
int log_in();
void User_remove();
void* notify_receive(void* arg);
string get_uid();
int Main_menu();
void User_menu();
void Friend_menu();
