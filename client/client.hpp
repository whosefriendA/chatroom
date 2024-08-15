#include"../TaskSocket.hpp"
#include"../Redis.hpp"
#include"../Error.hpp"
#include"../Message.hpp"
#include<openssl/sha.h>
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
#include<thread>
#define SERVERPORT 8080//默认端口号
#define RECV -1
#define SIGNUP 1
#define LOGIN 2
#define USER_REMOVE 3
#define SHIELD_FRIEND 4
#define RESTORE_FRIEND 5
#define SENDFILE_GROUP 6
#define RECVFILE_GROUP 7
#define FRIEND_LIST 8
#define ADD_FRIEND 9
#define DELETE_FRIEND 10
#define AGREE_ADDFRIEND 11
#define REFUSE_ADDFRIEND 12
#define F_SENDFILE 13
#define F_RECVFILE 14
#define QUESTION_GET 15
#define PASSWORD_FIND 16
#define PASSWORD_GET 17
#define CHAT_WITH_FRIEND 18
#define SEND_MSG 19
#define EXIT_CHAT 20
#define UNREAD_NOTICE 21
#define CREATE_GROUP 22
#define GROUP_LIST 23
#define ADD_GROUP 24
#define GROUP_IN 25
#define GROUP_APPLY_LIST 26
#define GROUP_APPLY_AGREE 27
#define GROUP_APPLY_REFUSE 28
#define GROUP_MEMBER_LIST 29
#define GROUP_MANAGER_SET 30
#define GROUP_MANAGER_UNSET 31
#define GROUP_DEL_MEMBER 32
#define GROUP_DISBAND 33
#define GROUP_EXIT 34
#define GROUP_CHAT 35
#define GROUP_SENDMSG 36
#define GRUOP_CHATEXIT 37
#define HEARTBEAT 38
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define WIDEWHITE "\033[1;37m"
using namespace std;
using json=nlohmann::json;
extern TaskSocket asocket;
extern sockaddr_in server_addr2;
extern Error err;
extern string curuid;
extern string curname;
extern string cur_groupuid;
void sendHeartbeat(TaskSocket);
int Main_menu();
void User_menu();
void Friend_menu();
void Group_menu();
void Agroup_menu();
void pass_find();
void sign_up();
int log_in();
int user_remove();
void* notify_receive(void* arg);
string get_uid();
void Sendflie_client(TaskSocket,string,string,int);
void Receiveflie_client(TaskSocket,string,string,int);
int friend_list_get();
void freind_add();
void friend_del();
void Friend_apply();
void friend_apply_agree();
void friend_apply_refuse();
void Friend_shield();
void friend_shield();
void friend_restore();
void friend_chat();
void Unreadnotice();
void group_create();
void group_add();
void group_in();
void group_list_get();
int group_disband();
void group_memberlist_get();
void group_apply_list();
void group_apply_agree();
void group_apply_refuse();
void group_manager_set();
void group_manager_unset();
void Group_apply();
void group_delmember();
void group_chat();
int group_exit();
