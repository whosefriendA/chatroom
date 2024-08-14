#include"server.hpp"
Redis redis;
unordered_set<string> online_users;
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
        server_addr.sin_port=std::stoi(optarg);
        break;
        }
    }
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
                if(epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&event)==-1){
                   perror("new event error");
                   exit(0);
                }
            }else {
                TaskSocket asocket(nfd);
                char *buf;
                int ret=asocket.Receive_server(nfd,&buf);
                cout<<"ret="<<ret<<endl;//
                if(ret<=0){//用户退出了
                    string uid =redis.Hget("fd-uid表",to_string(nfd));
                    online_users.erase(uid);//删除在线用户
                    redis.Hset(uid,"实时socket","-1");
                    redis.Hset("fd-uid表",to_string(nfd),"-1");
                    close(nfd);
                    continue;
                }
                buf[ret]='\0';
                // cout<<buf<<endl;//
                string comad_string=buf;
                cout << "New request:" << comad_string << endl<<endl;
                Message msg;
                msg.Json_to_s(comad_string);
                if(msg.flag==RECV){
                redis.Hset(msg.uid,"实时socket",to_string(nfd));
                }else if(msg.flag==F_SENDFILE||msg.flag==F_RECVFILE||msg.flag==SENDFILE_GROUP||msg.flag==RECVFILE_GROUP){
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
void transferfunc(TaskSocket asocket,const string& comad_string)
{   Message msg;
    msg.Json_to_s(comad_string);
    switch((int)msg.flag){
        case SIGNUP:
            Sign_up(asocket,msg);
            break;
        case LOGIN:
            Log_in(asocket,msg);
            break;
        case USER_REMOVE:
            User_remove(asocket,msg);
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
            friend_list_get(asocket,msg);
            break;
        case ADD_FRIEND:
            friend_add(asocket,msg);
            break;
        case DELETE_FRIEND:
            friend_del(asocket,msg);
            break;
        case AGREE_ADDFRIEND:
            friend_apply_agree(asocket,msg);
            break;
        case REFUSE_ADDFRIEND:
            friend_apply_refuse(asocket,msg);
            break;
        case BLOCK_FRIEND:
            friend_block(asocket,msg);
            break;
        case RESTORE_FRIEND:
            friend_restore(asocket,msg);
            break;
        case CHAT_WITH_FRIEND:
            friend_chat(asocket,msg);
            break;
        case EXITCHAT:
            Exitchat(asocket,msg);
            break;
        case SEND_MSG:
            friend_sendmsg(asocket,msg);
            break;
        case F_SENDFILE:
            F_sendfile_toserver(asocket,msg);
            break;
        case F_RECVFILE:
            F_receivefile_fromserver(asocket,msg);
            break;
        case UNREAD_NOTICE:
            Unreadnotice(asocket,msg);
            break;
        case CREATE_GROUP:
            group_create(asocket,msg);
            break;
        case ADD_GROUP:
            group_add(asocket,msg);
            break;
        case GROUP_IN:
            group_in(asocket,msg);
            break;
        case GROUP_LIST:
            group_list_get(asocket,msg);
            break;
        case GROUP_MEMBER_LIST:
            group_memberlist_get(asocket,msg);
            break;
        case GROUP_APPLY_LIST:
            group_apply_list(asocket,msg);
            break;
        case GROUP_APPLY_AGREE:
            group_apply_agree(asocket,msg);
            break;
        case GROUP_APPLY_REFUSE:
            group_apply_refuse(asocket,msg);
            break;
        case GROUP_MANAGER_SET:
           group_manager_set(asocket,msg);
           break;
        case GROUP_MANAGER_UNSET:
           group_manager_unset(asocket,msg);
           break;
        case GROUP_DISBAND:
           group_disband(asocket,msg);
           break; 
        case GROUP_EXIT:
            group_exit(asocket,msg);
            break;
        case GROUP_DEL_MEMBER:
            group_delmember(asocket,msg);
            break;
        case GROUP_CHAT:
            group_chat(asocket,msg);
            break;
        case GRUOP_CHATEXIT:
            group_chatexit(asocket,msg);
            break;
        case GROUP_SENDMSG:
            group_sendmsg(asocket,msg);
            break;
        case SENDFILE_GROUP:
            G_sendfile_toserver(asocket,msg);
            break;
        case RECVFILE_GROUP:
            G_receivefile_fromserver(asocket,msg);
            break;
    }
    return;
}
