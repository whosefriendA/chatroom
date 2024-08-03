#ifndef _SEVER_H_
#define _SEVER_H_
#include"../TaskSocket.hpp"
#include"../Redis.hpp"
#include"../Threadmanage.hpp"
#include"../Method.hpp"
#include"../Message.hpp"
#include<hiredis/hiredis.h>
#include<nlohmann/json.hpp>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<unistd.h>
#include<mutex>
#include<string>
#include<string.h>
#include<csignal>
#include<fcntl.h>
#include<hiredis/hiredis.h>
#include<iostream>
#include<queue>
#include<vector>
#include<thread>
#include<condition_variable>
#include<functional>
#include<netdb.h>
#include<atomic>
#define FID 1//始祖UID
#define SERVERPORT 8080//默认端口号
#define RECV -1
#define SIGNUP 1
#define LOGIN 2
#define LOGOUT 3
#define FRIEND_LIST 4
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
#define RED "\033[31m"
#define YELLOW "\033[33m"
#define RESET "\033[0m"
extern Redis redis;
extern Method method;
extern string online_users;
extern sockaddr_in server_addr;
extern socklen_t server_addr_len;
extern int server_fd;
extern int server_port;
extern int User_count;
extern int User_uid;
using std::string;
using std::mutex;
using json=nlohmann::json;

void Sign_up(TaskSocket,Message);
void Log_in(TaskSocket,Message);
void pass_find(TaskSocket,Message);
void question_get(TaskSocket,Message);
void pass_get(TaskSocket,Message);
void User_remove(TaskSocket, Message);
void friend_list_get(TaskSocket,Message);
void freind_add(TaskSocket,Message);
void friend_del(TaskSocket,Message);
void friend_apply_agree(TaskSocket,Message);
void friend_apply_refuse(TaskSocket,Message);
class User{
    public:
    User(string name,string pass,string question,string answer,string UID);
    string UID,Name,Pass,Question,Answer;
    json friend_list;
    json group_list;
    string user_inquire(string info);
    void user_mem();
    //好友相关
    void friend_add(string friend_id);
    void friend_delete(string friend_id);
    void friend_shield(string friend_id);
    void friend_recover(string friend_id);
    //群相关
    void group_add(string group_id);
    void group_delete(string group_id);
    void group_shield(string group_id);
    void group_recover(string group_id);
    private:
    mutex user_mutex;
};
void transferfunc(TaskSocket asocket, const std::string& comad_string);

#endif