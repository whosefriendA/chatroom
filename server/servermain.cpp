#include"server.hpp"
Redis redis;
unordered_set<string> online_users;
unordered_map<int, chrono::time_point<chrono::steady_clock>> client_last_active;
sockaddr_in server_addr;
socklen_t server_addr_len=sizeof(server_addr);
int server_port=SERVERPORT;
int User_uid=1001;
int Group_uid=101;
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
    // 命令行参数设置服务器地址
    int opt;
    while((opt=getopt(argc,argv,"i:p:"))!=-1){
        switch(opt){
        case 'i':
        server_addr.sin_addr.s_addr=inet_addr(optarg);
        break;
        case 'p':
        server_addr.sin_port=stoi(optarg);
        break;
        }
    }
    // cout<<server_addr.sin_addr.s_addr<<endl;
    // cout<<server_addr.sin_port<<endl;
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
    // if (getnameinfo((struct sockaddr*)&server_addr,sizeof(server_addr),host,sizeof(host),service,sizeof(service),NI_NUMERICHOST) == 0) {
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
    cout<<"服务器开始工作"<<endl;
    // cout<<"下面是uid列表"<<endl;
    // vector<string> members=redis.Sgetall("uid集合");
    // for(const string&member :members){
    //     cout<<member<<endl;
    // }
    thread heartbeatThread(checkHeartbeat, epfd);
    heartbeatThread.detach();

    while(true){
        int count =epoll_wait(epfd,recvevent,size,-1);
        for(int i=0;i<count;i++){
            // cout<<count<<endl;//
            int nfd=recvevent[i].data.fd;
            if(nfd==lfd){
                // cout<<"nfd=lfd"<<endl;
                cfd=accept(nfd,NULL,NULL);
                
                int flag=fcntl(cfd,F_GETFL);
                flag|=O_NONBLOCK;
                fcntl(cfd,F_SETFL,flag);

                event.events = EPOLLIN | EPOLLET; 
                event.data.fd=cfd;
                epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&event);
            }else {
                TaskSocket asocket(nfd);
                char *buf;
                int ret=asocket.Receive_server(nfd,&buf);
                // cout<<"ret="<<ret<<endl;//
                if(ret<=0){//用户退出了
                    client_dead(nfd);
                    continue;
                }
                // cout<<buf<<endl;
                string comad_string=buf;
                cout << "New request:" << comad_string << endl<<endl;
                Message msg;
                msg.Json_to_s(comad_string);
                if(msg.flag==RECV){
                redis.Hset(msg.uid,"实时socket",to_string(nfd));
                }else if(msg.flag==F_SENDFILE||msg.flag==F_RECVFILE||msg.flag==SENDFILE_GROUP||msg.flag==RECVFILE_GROUP){
                    client_last_active.erase(nfd);
                    event.events = EPOLLIN|EPOLLET;
                    event.data.fd = nfd;
                    //摘树
                    epoll_ctl(epfd,EPOLL_CTL_DEL,nfd,&event);
                    //文件传输
                    std::thread fileThread([asocket,comad_string ,nfd,epfd]() {
                        int flag = fcntl(nfd,F_GETFL);
                        flag |= O_NONBLOCK;
                        fcntl(nfd,F_SETFL,flag);
                        transferfunc(asocket,comad_string);
                        // 挂树
                        struct epoll_event addEvent;
                        addEvent.events = EPOLLIN | EPOLLET;
                        addEvent.data.fd = nfd;
                        epoll_ctl(epfd,EPOLL_CTL_ADD,nfd,&addEvent);
                    });
                    //分离
                    fileThread.detach();
                }else{
                    client_lastactive_now(nfd);
                    // cout<<"will create task to thread"<<endl;
                    TaskSocket socket(nfd);
                    // 使用 lambda 表达式创建带参数的 taskhandler并添加到调度器
                    Task task([socket,comad_string](){ 
                    transferfunc(socket,comad_string);});
                    pool.addTask(task);
                }
            }     
        }
    }
}

void checkHeartbeat(int epfd) {
    while (true) {
        auto now = chrono::steady_clock::now();
        for (auto it = client_last_active.begin(); it != client_last_active.end(); ) {

            if (chrono::duration_cast<chrono::seconds>(now - it->second).count() > 400) { // 180秒超时
                cout << "Client " << it->first << " 超时" << endl;
                close(it->first);
                client_dead(it->first);
                it = client_last_active.erase(it);
            } else {
                ++it;
            }
        }
        this_thread::sleep_for(chrono::seconds(300)); // 每5秒检查一次
    }
}
void client_dead(int nfd){
    string uid =redis.Hget("fd-uid表",to_string(nfd));
    online_users.erase(uid);//删除在线用户
    redis.Hset(uid,"实时socket","-1");
    redis.Hset("fd-uid表",to_string(nfd),"-1");
    close(nfd);
}
void client_lastactive_now(int nfd){
    string uid =redis.Hget("fd-uid表",to_string(nfd));
    if(stoi(uid)!=-1){
    client_last_active[nfd] = chrono::steady_clock::now();
    }
}