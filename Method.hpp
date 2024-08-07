#include"./Error.hpp"
#include<iostream>
#include<sys/sendfile.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<cerrno>
#include<cstring>
Error err;
class Method {
    public:
    Method()=default;
    ~Method()=default;
    void Sendflie_client(TaskSocket asocket,string curuid,string recvuid,int SENDFILE){
        string filepath;
        cout<<"请输入要发送文件的位置:"<<endl;
        getline(cin,filepath);
        int filefd=open(filepath.c_str(),O_RDONLY);
        if(filefd==-1){
            cerr<<"Error opening file"<<endl;
            return;
        }else{
            off_t filesize=lseek(filefd,0,SEEK_END);
            lseek(filefd, 0, SEEK_SET);
            size_t lastSlash=filepath.find_last_of("/");
            string filename=filepath.substr(lastSlash+1);//提取文件名

            Message msg_file(curuid,recvuid,SENDFILE,{filename,to_string(filesize)});
            int ret=asocket.Send(msg_file.S_to_json());
            err.server_close(ret);

            string recv_file=asocket.Receive();
            err.server_close(recv_file);
            if(recv_file=="success"){
                ssize_t offset = 0;
                cout<<filesize<<endl;
                while (offset < filesize) {
                    ssize_t sent_bytes = sendfile(asocket.getfd(), filefd, &offset, filesize - offset);
                    cout<<sent_bytes<<endl;
                    if (sent_bytes == -1){
                        if (errno == EAGAIN || errno == EWOULDBLOCK) {
                            continue;//如果是阻塞错误码重试
                        } else {
                            cerr << "Error sending file: " << strerror(errno) << endl;
                            close(filefd);
                            break;
                        }
                    } else if (sent_bytes == 0) {
                        cerr << "Connection closed while sending file." << endl;
                        break;
                    }
                }
            }
            close(filefd);
        }
        string recv=asocket.Receive();
        err.server_close(recv);
        if(recv=="success"){
            cout<<"文件上传成功"<<endl;
        }
    }
    void Receiveflie_client(TaskSocket asocket,string curuid,string recvuid,int RECVFILE){
                string filepath;

                cout<<"请输入想保存文件的位置:"<<endl;
                getline(cin,filepath);

                size_t lastSlash=filepath.find_last_of("/");//找到最后一个斜杠或者反斜杠
                string filename=filepath.substr(lastSlash+1);//获取到文件名

                Message msg_file(curuid,recvuid,RECVFILE,{filename});
                int ret=asocket.Send(msg_file.S_to_json());
                err.server_close(ret);

                string recv_file=asocket.Receive();
                err.server_close(recv_file);
                if(recv_file=="failure"){
                    cout<<"暂无未接收的文件"<<endl;
                    return;
                }else{
                    int filefd=open(filepath.c_str(),O_APPEND|O_WRONLY|O_CREAT,S_IRWXU);
                    if(filefd==-1){
                        cerr<<"Error opening file:"<<strerror(errno)<<endl;
                        return;
                    }
                    cout<<recv_file.c_str()<<endl;
                    ssize_t size=stoll(recv_file.c_str());//文件大小
                    cout<<size<<endl;
                    char buf[BUFSIZ];
                    ssize_t totalRecvByte=0;

                    lseek(filefd,0,SEEK_SET);
                    while(size>totalRecvByte){
                        cout<<"size:"<<size<<endl;
                        ssize_t read_byte=read(asocket.getfd(),buf,sizeof(buf));
                         if (read_byte == -1) {
                            if(errno == EAGAIN || errno == EWOULDBLOCK){
                                continue;
                            }else{
                                cerr << "Error reading file: " << strerror(errno) << endl;
                            }
                        }
                        if (read_byte == 0) {
                            cerr << "Connection closed by client" << endl;
                            break;
                        }
                        if (size==totalRecvByte) {
                            cout<<"size:"<<size<<endl;
                            cout<<"totalRecvByte:"<<totalRecvByte<<endl;
                            //cerr << "Connection closed by client" << endl;
                            break;
                        }
                        ssize_t Writeb=write(filefd,buf,read_byte);
                        if (Writeb == -1) {
                            cerr << "Error writing to file" << endl;
                            break;
                        }
                        totalRecvByte+=read_byte;
                        cout<<"Curtotal:"<<totalRecvByte<<endl;
                        cout << "byteRead:" << read_byte << endl;
                        cout << "bytewrite" << Writeb << endl;
                    }
                    if (totalRecvByte < size) {
                        cerr << "File transmission incomplete" << endl;
                    }
                    close(filefd);
                    cout<<"文件接收完毕"<<endl;
                }
    }
    int Receivemsg(int cfd,char** msg)
{
    int len=0;
    Read(cfd,(char *)&len,4);
    len=ntohl(len);
    printf("数据大小为%d\n",len);

    char *buf=(char *)malloc(len+1);
    int ret=Read(cfd,buf,len);
    if(ret!=len){
        printf("数据接收失败\n");
    }else if(ret==0){
        printf("对方断开连接\n");
        close(cfd);
    }
    buf[len]='\0';
    *msg=buf;
    return ret;
}
ssize_t Read (int fd,void *aptr,size_t n)
{
    size_t leftn;
    ssize_t readn;
    char *ptr;

    ptr=(char *)aptr;
    leftn=n;
    while(leftn>0)
    {
        if((readn=read(fd,ptr,leftn))<0){
            if(errno==EINTR||EWOULDBLOCK){
                readn=0;
            }else{
                return -1;
            }
        }else if(readn==0){
            break;
        }
        leftn-readn;
        ptr+=readn;
    }
    return n-leftn;
}
};