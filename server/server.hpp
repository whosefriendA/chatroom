#ifndef _SEVER_H_
#define _SEVER_H_
#include"../TaskSocket.hpp"
#include"../Redis.hpp"
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
extern Redis redis;
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

void Sign_up(TaskSocket,json);
void Log_in(TaskSocket,json);
void pass_find(TaskSocket,json);
void question_get(TaskSocket,json);
void pass_get(TaskSocket,json);
void User_remove(TaskSocket, json);
int recvMsg(int cfd,char** msg);
ssize_t Readn(int fd,void *vptr,size_t n);
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
ssize_t Read (int fd,void *vptr,size_t n);
void transferfunc(TaskSocket asocket, const std::string& comad_string);

#endif