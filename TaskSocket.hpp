#ifndef _TASKSOCKET_H_
#define _TASKSOCKET_H_
#include<iostream>
#include<arpa/inet.h>
#include<string>
#include<cstring>
#include<unistd.h>
#include<sys/socket.h>
#include<json/json.h>

using namespace  std;

class TaskSocket
{
public:
    TaskSocket(){
    tfd=socket(AF_INET,SOCK_STREAM,0);
};
    TaskSocket(int fd):tfd(fd){}
    TaskSocket(string msg){
    if(msg=="recv")
    {
        tfd=socket(AF_INET,SOCK_STREAM,0);
        recv_fd=socket(AF_INET,SOCK_STREAM,0);    
    }
}

    ~TaskSocket(){};

    int getfd(){return tfd;}
    int getresvfd(){return recv_fd;}


int TaskSocket::Sendmsg(string msg){
    char *data=new char[msg.size()+4];
    int biglen=htonl(msg.size());
    
    memcpy(data,&biglen,4);//数据头
    memcpy(data+4,msg.data(),msg.size());//数据体

    int ret=Writemsg(data,msg.size()+4);//发送数据

    delete[]data;//释放内存

    return ret;
}


int TaskSocket::Writemsg(const char *msg,int size){
    const char *buf=msg;
    int cnt=size;
    while(cnt>0){
        int len=send(tfd,buf,cnt,0);//记录发送的字节数
        if(len==-1)
        {
            close(tfd);
            perror("writen error");
            exit(0);

        }else if(len==0)
        {
            continue;
        }

        buf+=len;
        cnt-=len;//计算剩余量
    }
    return size;//返回成功发送的字节数
}


string TaskSocket::Recvmsg(){
    //数据头
    int len=0;
    int ret=Readmsg((char *)&len,4);
    if(ret==0){
        return "close1";
    }
    len=ntohl(len);

    char *buf=new char[len+1];
    // 数据体
    ret=Readmsg(buf,len);
    if(ret!=len)
    {
        return to_string(len);
    }else if(ret==0)
    {
        close(tfd);
        return "close";
    }

    buf[len]='\0';
    string msg(buf);
    delete[]buf;

    return msg;
}


int TaskSocket::Readmsg(char *buf,int size){
    char *pt=buf;
    int cnt=size;

    while(cnt>0)
    {
        int len=recv(tfd,pt,cnt,0);
        if(len==-1)
        {
            close(tfd);
            perror("read error");
            exit(0);
        
        }else if(len==0)//表明连接结束
        {
            cout<<"连接结束"<<endl;
            close(tfd);
            return size-cnt;
        }

        pt+=len;
        cnt-=len;
    }

    return size-cnt;

}

private:
    int tfd=-1;//向服务器发送消息
    int recv_fd=-1;//接收服务器发送的消息
};


#endif