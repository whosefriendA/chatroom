#include<iostream>
class Method {
    public:
    Method()=default;
    ~Method()=default;
    int recvMsg(int cfd,char** msg)//接受带数据头的数据包
{
    //接收数据头
    int len=0;
    Read(cfd,(char *)&len,4);
    len=ntohl(len);
    printf("数据块大小为%d\n",len);

    char *buf=(char *)malloc(len+1);//留出存储'\0'的位置
    int ret=Read(cfd,buf,len);
    if(ret!=len)
    {
        printf("数据接收失败\n");
    }else if(ret==0){
        printf("对方断开连接\n");
        close(cfd);
    }

    buf[len]='\0';
    *msg=buf;

    return ret;
}
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
};