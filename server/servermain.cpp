#include"server.hpp"
Redis redis;
Method method;
string online_users;
sockaddr_in server_addr;
socklen_t server_addr_len=sizeof(server_addr);
int server_port=SERVERPORT;
int User_count=0;
int User_uid=1001;
using namespace std;

int cfd;
int main(int argc,char*argv[]){
    signal(SIGPIPE,SIG_IGN);
    int lfd=0,cfd=0;
    char *buf;
    //设置默认服务器地址
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    server_addr.sin_port=htons(SERVERPORT);
    //命令行参数设置服务器地址
    // int opt;
    // while((opt=getopt(argc,argv,"i:p:"))!=-1){
    //     switch(opt){
    //     case 'i':
    //     server_addr.sin_addr.s_addr=inet_addr(optarg);
    //     break;
    //     case 'p':
    //     server_addr.sin_port=std::stoi(optarg);
    //     break;
    //     }
    // }
    //创建服务器套接字
    if((lfd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("sever_socket error");
    };
    //端口复用
    int sopt=1;
    setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&sopt,sizeof(sopt));

    //绑定
    if(bind(lfd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        perror("server_socket bind error");
        close(lfd);
        exit(0);
    }
    char host[1024];
    // char service[20];
    // if (getnameinfo((struct sockaddr*)&server_addr, sizeof(server_addr), host, sizeof(host), service, sizeof(service), NI_NUMERICHOST) == 0) {
    //     std::cout << "Server IP address: " << host << std::endl;
    // }测试ip地址
    if(listen(lfd,150)==-1){
        perror("server_socket error");
        close(lfd);
        exit(0);
    }
    struct epoll_event event,recvevent[1024];
    int size=sizeof(recvevent)/sizeof(struct epoll_event);

    int epfd=epoll_create1(0);
    if(epfd==-1){
        perror("epoll create error");
        exit(0);
    }
    event.events=EPOLLIN|EPOLLET;
    event.data.fd=lfd;
    int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,lfd,&event);
    if(ret==-1){
        perror("epoll ctrl error");
        exit(0);
    }
    ThreadPool pool(10);
    cout<<"下面是uid列表"<<endl;
    vector<string> members=redis.Sgetall("用户uid集合");
    for(const string&member :members){
        cout<<member<<endl;
    }
    while(true){
        int count =epoll_wait(epfd,recvevent,size,-1);
        for(int i=0;i<count;i++){
            //cout<<count<<endl;
            int nfd=recvevent[i].data.fd;
            if(nfd==lfd){
                //cout<<"nfd=lfd"<<endl;
                cfd=accept(nfd,NULL,NULL);
                
                int flag=fcntl(cfd,F_GETFL);
                flag|=O_NONBLOCK;
                fcntl(cfd,F_SETFL,flag);

                event.events = EPOLLIN | EPOLLET; 
                event.data.fd=cfd;
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&event)==-1){
                   perror("new event error");
                   exit(0);
                }
            }
            else {
                TaskSocket asocket(nfd);
                char *buf;
                int ret=method.recvMsg(nfd,&buf);
                //cout<<ret<<endl;
                if(ret<=0){
                    cerr<<"error receiving data ."<<endl;
                    string uid =redis.Hget("fd-uid表",to_string(nfd)); // 获取客户端的用户ID
                    // 添加到在线用户
                    redis.Sadd("onlie_users",uid);
                    redis.Hset(uid,"通知套接字","-1");
                    redis.Hset("fd-uid表",to_string(nfd),"-1");
                    close(nfd);
                    continue;
                }
                cout<<buf<<endl;
                string comad_string=buf;
                json data=json::parse(comad_string);
                if(data.at("flag")==RECV){
                redis.Hset(data.at("UID"), "通知套接字", to_string(nfd));
                }else if(data.at("flag")==SENDFILE||data.at("flag")==RECVFILE||data.at("flag")==SENDFILE_GROUP||data.at("flag")==RECVFILE_GROUP){
                    event.events = EPOLLIN|EPOLLET; // 不监听任何事件
                    event.data.fd = nfd;
                    epoll_ctl(epfd, EPOLL_CTL_DEL, nfd, &event);
                    //为文件传输创新线程
                    std::thread fileThread([asocket,comad_string ,nfd, epfd]() {
                        // 设置非阻塞
                        int flag = fcntl(nfd, F_GETFL);
                        flag |= O_NONBLOCK;
                        fcntl(nfd, F_SETFL, flag);
                        transferfunc(asocket,comad_string);
                        // 文件传输操作完成后，重新加入 epoll 
                        struct epoll_event addEvent;
                        addEvent.events = EPOLLIN | EPOLLET;
                        addEvent.data.fd = nfd;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, nfd, &addEvent);
                    });
                    //分离文件传输线程
                    fileThread.detach();
                    }
                    else{
                        cout<<"will create task to thread"<<endl;
                    TaskSocket socket(nfd);
                    // 使用 lambda 表达式创建带参数的 taskhandler并添加到调度器
                    Task task([socket, comad_string](){ 
                    transferfunc(socket, comad_string);});
                    pool.addTask(task);
        }
    }
}
}
}
void transferfunc(TaskSocket asocket, const string& comad_string)
{   Message msg;
    msg.Json_to_s(comad_string);
    switch((int)msg.flag){
        case SIGNUP:
            Sign_up(asocket,msg);
            break;
        case LOGIN:
            Log_in(asocket, msg);
            break;
        case QUESTION_GET:
            question_get(asocket,msg);
            break;
        case PASSWORD_FIND:
            pass_find(asocket,msg);
            break;
        case PASSWORD_GET:
            pass_get(asocket,msg);
            break;
        case FRIEND_LIST:
            friend_list_get(asocket, msg);
            break;
        case ADD_FRIEND:
            freind_add(asocket, msg);
            break;
        case DELETE_FRIEND:
            friend_del(asocket, msg);
            break;
        case AGREE_ADDFRIEND:
            friend_apply_agree(asocket, msg);
            break;
        case REFUSE_ADDFRIEND:
            friend_apply_refuse(asocket, msg);
            break;
        case BLOCK_FRIEND:
            break;
        case RESTORE_FRIEND:
            break;
        case SENDFILE:
            break;
        case RECVFILE:
            break;

    }

    return;
}