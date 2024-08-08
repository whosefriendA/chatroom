#include"../TaskSocket.hpp"
#include"../Redis.hpp"
#include"../Error.hpp"
#include"../Message.hpp"
#include"../Method.hpp"
#include<signal.h>
#include<iostream>
#include<chrono>
#include<fcntl.h>
#include<sys/sendfile.h>
#include<sys/stat.h>
#include<unistd.h>
#include<termios.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#include<nlohmann/json.hpp>
#include<hiredis/hiredis.h>
#define SERVERPORT 8080//默认端口号
#define RECV -1
#define SIGNUP 1
#define LOGIN 2
#define LOGOUT 3
#define BLOCK_FRIEND 4
#define RESTORE_FRIEND 5
#define SENDFILE_GROUP 6
#define RECVFILE_GROUP 7
#define FRIEND_LIST 8
#define ADD_FRIEND 9
#define DELETE_FRIEND 10
#define AGREE_ADDFRIEND 11
#define REFUSE_ADDFRIEND 12
#define SENDFILE 13
#define RECVFILE 14
#define QUESTION_GET 15
#define PASSWORD_FIND 16
#define PASSWORD_GET 17
#define CHAT_WITH_FRIEND 18
#define SEND_MSG 19
#define EXIT_CHAT 20
#define UNREAD_NOTICE 21
#define GETNAME 22
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
using namespace std;
using json=nlohmann::json;
extern TaskSocket asocket;
extern sockaddr_in server_addr2;
extern Method method;
extern Error err;
extern string curuid;
extern string curname;
int Main_menu();
void User_menu();
void Friend_menu();
void pass_find();
void sign_up();
int log_in();
void* notify_receive(void* arg);
string get_uid();
void friend_list_get();
void freind_add();
void friend_del();
void Friend_apply();
void friend_apply_agree();
void friend_apply_refuse();
void Friend_block();
void friend_block();
void friend_restore();
void friend_chat();
void Unreadnotice();
void group_create();
void group_add();
void group_list_get();
void group_disband();
void group_memberlist_get();
void group_apply();
void group_manager_set();
void group_manager_unset();