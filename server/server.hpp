#ifndef _SEVER_H_
#define _SEVER_H_
#include"../TaskSocket.hpp"
#include<hiredis/hiredis.h>
#include<nlohmann/json.hpp>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/socket.h>
#include<unistd.h>
#include<mutex>
#include<string>
#include<csignal>
#include<fcntl.h>
#include<hiredis/hiredis.h>
#include<iostream>
#include<queue>
#include<vector>
#include<thread>
#include<condition_variable>
#include<functional>
#define FID 1001//始祖UID
#define SERVERPORT 8080//默认端口号
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
string online_users;
sockaddr_in server_addr;
socklen_t server_addr_len=sizeof(server_addr);
int server_fd;
int server_port;
int User_count=0;

using std::string;
using std::mutex;
using json=nlohmann::json;

void Sign_up(TaskSocket,json);
void Log_in(TaskSocket,json);
void Log_out(TaskSocket,json);


class User{
    public:
    User(string name,string pass,string question,string answer);
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

ssize_t Read (int fd,void *vptr,size_t n)
{
    size_t nleft;
    ssize_t nread;
    char *ptr;

    ptr=(char *)vptr;
    nleft=n;

    while(nleft>0)
    {
        if((nread=read(fd,ptr,nleft))<0)
        {
            if(errno==EINTR||EWOULDBLOCK)
            {
                nread=0;
            }else{
                return -1;
            }
        }else if(nread==0)
        {
            break;
        }

        nleft-=nread;
        ptr+=nread;
    }

    return n-nleft;
}

void taskhandler(TaskSocket mysocket, const std::string& comad_string)
{
    json command =json::parse(comad_string);
    switch((int)command.at("flag")){
        case SIGNUP:
            Sign_up(mysocket,command);
            break;
        case LOGIN:
            Log_in(mysocket, command);
            break;
        case LOGOUT:
            Log_out(mysocket, command);
            break;
    }

    return;
}

#endif