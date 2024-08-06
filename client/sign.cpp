#include"./client.hpp"
void sign_up(){
    string name,pass,pass2;
    string question,answer;

    while(1){

        cout<<"请输入密码:"<<endl;
        getline(cin,pass);

        while(pass.size()==0){
            cout<<"请重新输入有效的密码:"<<endl;
            getline(cin,pass);
        }

        cout<<"请确认密码:"<<endl;
        getline(cin,pass2);
        if(pass!=pass2){
            system("clear");
            cout<<"密码不一致"<<endl;
        }else break;
    }
    cout<<"请输入昵称:"<<endl;
    getline(cin,name);
    cout<<"设置密保问题"<<endl;
    getline(cin,question);
    cout<<"设置答案"<<endl;
    getline(cin,answer);
    Message msg(name,question,SIGNUP,pass,{answer});
    int ret=asocket.Sendmsg(msg.S_to_json());
    err.server_close(ret);

    string uid=asocket.Recvmsg();//收到生成的uid
    if(uid=="close"){
        cout<<"已关闭"<<endl;
        exit(0);
    }
    cout<<"你注册的uid为:"<<uid<<endl;
    cout<<"请登录"<<endl;

    return ;
}
string get_uid()
{
    string uid;
    int flag=1,c;

    cout<<"你的uid为:"<<endl;
    cin.ignore();
    getline(cin,uid);//获取输入的uid
    for(int c:uid){
        if(!isdigit(c)){
            flag=0;
            break;
        }
    }
    while(flag==0){
        cout<<"输入的uid有非法字符,请重新输入:"<<endl;
        cin.ignore();
        getline(cin,uid);
        flag=1;
        for(int c:uid){
            if(!isdigit(c)){
                flag=0;
                break;
            }
        }
    }
    return uid;
    
}
void* notify_receive(void* arg) {
    struct ThreadParams {
        std::string uid;
        int recv_fd;
    };
    ThreadParams* params = static_cast<ThreadParams*>(arg);
    TaskSocket recvsocket(params->recv_fd);
   if(connect(params->recv_fd,(sockaddr *)&server_addr2,sizeof(server_addr2))){
        perror("connect error");
        exit(0);
    }
    Message msg(params->uid,-1);
    int ret = recvsocket.Sendmsg(msg.S_to_json());
    if (ret == -1 || ret == 0) {
        std::cout << "服务器已关闭" << std::endl;
        delete params;
        exit(0);
    }
    while (true) {
        std::string recv = recvsocket.Recvmsg(); 
        if (recv == "close") {
            std::cout << "服务器已关闭" << std::endl;
            delete params;
            exit(0);
        }
        std::cout << recv << std::endl;
    }
    return nullptr;
}
int log_in(){
    string uid,pass;
    uid=get_uid();
    curuid=stoi(uid);
    cout<<"请输入您的密码:"<<endl;
    getline(cin,pass);
    Message msg(uid,LOGIN,{pass});
    int ret=asocket.Sendmsg(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Recvmsg();//接收返回的结果
    cout<<recv<<endl;
    err.server_close(recv);
    if(recv=="notcorrect"){
        cout<<"密码错误："<<endl;
        return 0;
    }else if(recv=="havevnotsignup"){
        cout<<"帐号不存在,请注册"<<endl;
        return 0;
    }else if(recv=="success"){
        system("clear");
        cout<<"登录成功"<<endl;
    pthread_t tid;
    // 传递参数
    struct ThreadParams {
        std::string uid;
        int recv_fd;
    };
    ThreadParams* params = new ThreadParams{uid, asocket.getresvfd()};

    // 处理通知的线程
    pthread_create(&tid, NULL, &notify_receive, static_cast<void*>(params));
    pthread_detach(tid);
        return 1;
    }
    return 1;
}

void pass_find()
{
    string uid,pass,answer;
    uid=get_uid();
    Message msg(uid,QUESTION_GET);
    int ret=asocket.Sendmsg(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Recvmsg();
    err.server_close(recv);
    cout<<recv<<endl;
    getline(cin,answer);
    Message msg(uid,PASSWORD_FIND);
    ret=asocket.Sendmsg(msg.S_to_json());
    err.server_close(ret);

    recv=asocket.Recvmsg();//接收返回的结果
    err.server_close(recv);
    if(recv!=answer){
        cout<<"答案不正确,无法找回"<<endl;
        return ;
    }else if(recv==answer)
    {
    Message msg(uid,PASSWORD_GET);
    ret=asocket.Sendmsg(msg.S_to_json());
    err.server_close(ret);

    recv=asocket.Recvmsg();
    cout<<"您的密码是："<<recv<<"请牢记您的密码"<<endl;
    return ;
    }
}