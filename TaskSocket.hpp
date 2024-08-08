#ifndef _TASKSOCKET_H_
#define _TASKSOCKET_H_
#include<iostream>
#include<arpa/inet.h>
#include<string>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
using namespace  std;

class TaskSocket
{
public:
    TaskSocket(){
    tfd=socket(AF_INET,SOCK_STREAM,0);
};
    TaskSocket(int fd):tfd(fd){}
    TaskSocket(string msg){
    if(msg=="receive"){
        tfd=socket(AF_INET,SOCK_STREAM,0);
        recv_fd=socket(AF_INET,SOCK_STREAM,0);    
    }
}
    ~TaskSocket(){};

int Send(string msg) {
    int msg_size = msg.size();
    int biglen = htonl(msg_size);
    char *data = new char[msg_size + 4];
    // 数据头
    memcpy(data, &biglen, 4);
    // 数据体
    memcpy(data + 4, msg.data(), msg_size);
    // 发送数据
    const char *buf = data;
    int cnt = msg_size + 4;
    while (cnt > 0) {
        int len = send(tfd, buf, cnt, 0);
        if (len == -1) {
            close(tfd);
            perror("write error");
            delete[] data;
            exit(0);
        } else if (len == 0) {
            continue;
        }
        buf += len;
        cnt -= len; 
    }
    delete[] data;
    return msg_size + 4;
}


string Receive() {
    // 数据头
    int len = 0;
    char *buf = new char[4];
    int cnt = 4;
    char *pt = (char *)&len;
    while (cnt > 0) {
        int ret = recv(tfd, pt, cnt, 0);
        if (ret == -1) {
            close(tfd);
            perror("read error");
            exit(0);
        } else if (ret == 0) { 
            cout << "连接已结束" << endl;
            close(tfd);
            delete[] buf;
            return "close";
        }
        pt += ret;
        cnt -= ret;
    }
    len = ntohl(len);
    delete[] buf; 
    buf = new char[len + 1];
    cnt = len;
    pt = buf;

    while (cnt > 0) {
        int ret = recv(tfd, pt, cnt, 0);
        if (ret == -1) {
            close(tfd);
            perror("read error");
            delete[] buf;
            exit(0);
        } else if (ret == 0) { 
            cout << "连接已结束" << endl;
            close(tfd);
            delete[] buf;
            return "close";
        }
        pt += ret;
        cnt -= ret;
    }
    buf[len] = '\0';
    string msg(buf);
    delete[] buf;

    return msg;
}
    int getfd(){return tfd;}
    int get_recvfd(){return recv_fd;}

    private:
    int tfd=-1;
    int recv_fd=-1;
    };


#endif