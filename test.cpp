#include<iostream>
#include<cstring>
#include<cstdlib>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<hiredis/hiredis.h>
#include<mutex>
#include<json/json.h>
// int main(){
//     redisContext *c=redisConnect("127.0.0.1",6379);
//     if(c->err){
//          printf("conect error");
//          redisFree(c);
//          return 0;
//     }
//     redisReply *reply=(redisReply *)redisCommand(c,"set foo 1234");
//     freeReplyObject(reply);
//     reply=(redisReply*)redisCommand(c,"get foo");
//     std::cout<<reply->str;
//     freeReplyObject(reply);
//     redisFree(c);
// }
// int port=12345;
// int buffersize=1024;
// int main(){
//     int server_fd,client_fd;
//     struct sockaddr_in server_addr,client_addr;
//     socklen_t client_len;
//     char buffer[buffersize];

//     if((server_fd=socket(AF_INET,SOCK_STREAM,0))==-1){
//         perror("socket");
//         return -1;
//     }
//     server_addr.sin_family=AF_INET;
//     server_addr.sin_addr.s_addr=INADDR_ANY;
//     server_addr.sin_port=htons(port);

//     if(bind(server_fd,(struct sockaddr*)&server_addr,sizeof(server_addr))==-1){
//         perror("bind");
//         return -1;
//     }
//     if(listen(server_fd,5)==-1){
//         perror("listen");
//         return -1;
//     }
//     while(true){
//         if((client_fd=accept(server_fd,(struct sockaddr*)&client_addr,&client_len))){
//             perror("accept");
//             return -1;
//         }
//         ssize_t bytes_re;
//         while(bytes_re=recv(client_fd,buffer,buffersize,0)>0){

//         }
//         close(client_fd);
//     }
//     close(server_fd):
//     }
// struct epoll_event ev,events[10];
// int epfd=epoll_create1(0);
// ev.events=EPOLLIN;
// ev.data.fd=server_fd
// epoll_ctl(int epfd,EPOLL_CTL_ADD,server_fd,&ev)
// while(true){
    
// }


//                 }else{
//                     // 创建任务并添加到线程池
//                     Argc_func* argc_func = new Argc_func(TcpSocket(curfd), command_string);
//                     Task task(taskhandler, argc_func);
//                     pool.addTask(task);

//                     // 从 epoll 模型中移除文件描述符
                    
//                     /*event.data.fd = curfd;
//                     epoll_ctl(efd, EPOLL_CTL_DEL, curfd, &event);

//                     // 创建 Argc_func 对象并将其传递给线程池
//                     Argc_func *argc_func = new Argc_func(TcpSocket(curfd), command_string);
//                     Task task(taskhandler, argc_func);
//                     pool.addTask(task);

//                     //上符
//                     event.data.fd=mysocket.getfd();
//                     event.events=EPOLLIN|EPOLLET;
//                     epoll_ctl(efd,EPOLL_CTL_ADD,event.data.fd,&event);*/
//                 }
                
//             }
//         }
        
//     } 
    
// }



// }

// //销毁账户
// void Log_out(TcpSocket mysocket,UserCommand command)//功能已实现
// {
//     // 获取该用户的好友列表
//     vector<string> friendList = redis.getFriendList(command.m_uid, "的好友列表");

//     for (const string& friendID : friendList) {
//         // 更新其他用户的好友列表，将该用户从好友列表中移除
//         redis.removeMember(friendID+"的好友列表", command.m_uid);
//     }

//     //移除该用户的好友列表
//     redis.delKey(command.m_uid+"的好友列表");
//     //移除该用户的屏蔽列表
//     redis.delKey(command.m_uid+"的屏蔽列表");

//     //删除uid当中存储的特殊的信息
//     redis.delKey(command.m_uid);//这一步成功执行
//     redis.delKey(command.m_uid+"的未读消息");//这一步成功执行
//     redis.delKey(command.m_uid+"的通知消息");

//     //这一步已实现，要删除集合中特定元素的值
//     redis.sremValue("用户uid集合", command.m_uid); //从用户uid集合中移除
//     mysocket.SendMsg("ok");

// }

// void Pass_re(TcpSocket mysocket,UserCommand command)
// {
//     string msg=redis.gethash(command.m_uid,"密保答案");
//     mysocket.SendMsg(msg);
//     return;
// }

// void ChangePwd(TcpSocket mysocket,UserCommand command)
// {
//     redis.hsetValue(command.m_uid,"密码",command.m_option[0]);
//     mysocket.SendMsg("ok");
//     return;
// }







/*struct UserCommand{
public:
    UserCommand()=default;
    ~UserCommand()=default;
    UserCommand(string uid,string question,string recvuid,int flag,vector<string> option)
    : m_uid(uid),m_question(question),m_recvuid(recvuid), m_flag(flag), m_option(option)
    {

    }

    string m_uid;//用户ID，默认为空字符串
    string m_recvuid;//接收者的uid
    string m_question;//用户密保问题的答案
    int m_flag;//用户发送操作者的类别
    vector<string> m_option;//命令的具体内容，动态数组

    void From_Json(string js)//从Json字符串中解析数据并填充到UserCommand结构体中
    {
        try {
            json jn = json::parse(js);
            jn.at("uid").get_to(m_uid);
            jn.at("question").get_to(m_question);
            jn.at("recvuid").get_to(m_recvuid);
            jn.at("flag").get_to(m_flag);
            jn.at("option").get_to(m_option);
        } catch (const exception& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
        }
    }

    string  To_Json()
    {
        json jn=json{
            {"uid",m_uid},
            {"question",m_question},
            {"recvuid",m_recvuid},
            {"flag",m_flag},
            {"option",m_option},
        };

        return jn.dump();//json格式转为json字符串格式
    }

};
*/