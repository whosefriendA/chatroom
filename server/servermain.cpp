#include"server.hpp"
Redis redis;
string online_users;
sockaddr_in server_addr;
socklen_t server_addr_len=sizeof(server_addr);
int server_fd=-1;
int server_port=SERVERPORT;
int User_count=0;
//任务类
class Task{
    public:
    Task(std::function<void()>func):func_(func){}
    void operator()() {func_();}
    private:
    std::function<void()>func_;
};
//线程池类
class ThreadPool{
    public:
    ThreadPool(int threadnum):stop_(false){
    if(threadnum<1)
    for(int i=0;i<threadnum;i++){
        workers_.emplace_back([this] {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex_);
                        condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });
                        if (stop_ && tasks_.empty())
                            return;
                        task = std::move(tasks_.front());
                        tasks_.pop();
                    }
                    task();
                }
            });
        }
    }

    template<class F>
    void enqueue(F&& task) {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            tasks_.emplace(std::forward<F>(task));
        }
        condition_.notify_one();
    }

    ~ThreadPool() {
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);
            stop_ = true;
        }
        condition_.notify_all();
        for (std::thread &worker : workers_)
            worker.join();
    }

    private:
    std::queue<std::function<void()>> tasks_;
    std::vector<std::thread> workers_;
    std::condition_variable condition_;
    std::mutex queue_mutex_;
    bool stop_;
};

//任务调度类
class TaskScheduler {
public:
    TaskScheduler(size_t numThreads) : threadPool_(numThreads) {}
    template<class F>
        void addTask(F&& task) {
            threadPool_.enqueue(Task(std::forward<F>(task)));
        }

private:
    ThreadPool threadPool_;
};


int cfd;
int main(int argc,char*argv[]){
    signal(SIGPIPE,SIG_IGN);
    // redisReply* reply1=(redisReply*)redisCommand(redis.con,"SADD %s %s",online_users.c_str(),"temp_member");
    // redisReply* reply2=(redisReply*)redisCommand(redis.con,"SREM %s %s",online_users.c_str(),"temp_member");
    // //创建服务器套接字
    if((server_fd=socket(AF_INET,SOCK_STREAM,0))==-1){
        perror("sever_socket error");
    };
    //端口复用
    int sopt=1;
    setsockopt(server_fd,SOL_SOCKET,SO_REUSEADDR,&sopt,sizeof(sopt));
    //设置默认服务器地址
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=INADDR_ANY;
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
    //绑定
    if(bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
        perror("server_socket bind error");
        close(server_fd);
        exit(0);
    }
    char host[1024];
// char service[20];
// if (getnameinfo((struct sockaddr*)&server_addr, sizeof(server_addr), host, sizeof(host), service, sizeof(service), NI_NUMERICHOST) == 0) {
//     std::cout << "Server IP address: " << host << std::endl;
// }测试ip地址
    if(listen(server_fd,150)==-1){
        perror("server_socket error");
        close(server_fd);
        exit(0);
    }
    struct epoll_event event,recvevent[1024];
    int size=sizeof(recvevent)/sizeof(struct epoll_event);

    int epfd=epoll_create1(0);
    event.events=EPOLLIN|EPOLLET;
    event.data.fd=server_fd;
    if(int ret=epoll_ctl(epfd,EPOLL_CTL_ADD,server_fd,&event)==-1){
        perror("epoll ctrl error");
        exit(0);
    }
    TaskScheduler scheduler(10); 
    while(true){
        int count =epoll_wait(epfd,recvevent,1024,-1);
        for(int i=0;i<count;i++){
            if(recvevent[i].data.fd==server_fd){
                cfd=accept(server_fd,(struct sockaddr *)&server_addr,&server_addr_len);
                int flag=fcntl(cfd,F_GETFL);
                flag|=O_NONBLOCK;
                fcntl(cfd,F_SETFL,flag);
                event.data.fd=cfd;
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&event)==-1){
                   perror("new event error");
                   exit(0);
                }
            }
            else {
                int nfd=recvevent[i].data.fd;
                
                TaskSocket  asocket(nfd);
                char *buf;
                int ret=recvMsg(nfd,&buf);
                if(ret<=0){
                    cerr<<"error receiving data ."<<endl;
                    // freeReplyObject(reply1);
                    // freeReplyObject(reply2);
                    redisReply *reply1=(redisReply*)redisCommand(redis.con,"HGET %s %s","fd_uid表",to_string(nfd));
                    string UID=reply1->str;
                    void *reply2=redisCommand(redis.con,"SADD %s %s",online_users.c_str(),UID);
                    freeReplyObject(reply1);
                    freeReplyObject(reply2);
                    void* reply=redisCommand(redis.con,"HSET %s %s %s","UID","通知socket","-1");
                    freeReplyObject(reply);
                    reply=redisCommand(redis.con,"HSET %s %s %s","fd_uid表",to_string(recvevent[i].data.fd),"-1");
                    freeReplyObject(reply);
                }
                buf[ret]='/0';
                string comad_string=buf;
                json data=json::parse(comad_string);
                    if(data.at("flag")==RECV){
                    redisReply* reply=(redisReply*)redisCommand(redis.con,"HGET %s %s %s",data.at("UID"), "通知套接字", to_string(nfd));
                    freeReplyObject(reply);
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
                        taskhandler(asocket,comad_string);
                        // 文件传输操作完成后，重新加入 epoll 
                        struct epoll_event addEvent;
                        addEvent.events = EPOLLIN | EPOLLET;
                        addEvent.data.fd = nfd;
                        epoll_ctl(epfd, EPOLL_CTL_ADD, nfd, &addEvent);
                    });

                    // 等待线程执行完毕
                    fileThread.detach();
            }
        }
    }
}
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

void taskhandler(TaskSocket asocket, const std::string& comad_string)
{
    json command =json::parse(comad_string);
    switch((int)command.at("flag")){
        case SIGNUP:
            Sign_up(asocket,command);
            break;
        case LOGIN:
            Log_in(asocket, command);
            break;
        case QUESTION_GET:
            question_get(asocket,command);
            break;
        case PASSWORD_FIND:
            pass_find(asocket,command);
            break;
        case PASSWORD_GET:
            pass_get(asocket,command);\
            break;
    }

    return;
}
int recvMsg(int cfd,char** msg)//接受带数据头的数据包
{
    //接收数据头
    int len=0;
    Readn(cfd,(char *)&len,4);
    len=ntohl(len);
   // printf("数据块大小为%d\n",len);

    char *buf=(char *)malloc(len+1);//留出存储'\0'的位置
    int ret=Readn(cfd,buf,len);
    /*if(ret!=len)
    {
        printf("数据接收失败\n");
    }else if(ret==0){
        printf("对方断开连接\n");
        close(cfd);
    }*/

    buf[len]='\0';
    *msg=buf;

    return ret;//返回接收的字节数
}

ssize_t Readn(int fd,void *vptr,size_t n)
{
    size_t nleft;//usigned int剩余未读取的字节数
    ssize_t nread;//int实际读到的字节数
    char *ptr;

    ptr=(char *)vptr;
    nleft=n;//n是未读取的字节数

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