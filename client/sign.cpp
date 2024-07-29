#include"./client.hpp"
void sign_up(){
    string name,pass,pass2;
    string question,answer;

    cout<<"请输入昵称"<<endl;
    getline(cin,name);
    while(1){

        cout<<"请输入密码:"<<endl;
        getline(cin,pass);

        while(pass.size()==0){
            cout<<"请重新输入有效的密码:"<<endl;
            getline(cin,pass);
        }

        cout<<"请确认密码:"<<endl;
        getline(cin,pass);
        if(pass!=pass2){
            system("clear");
            cout<<"密码不一致"<<endl;
        }else break;
    }

    cout<<"设置您的密保问题"<<endl;
    getline(cin,question);
    cout<<"设置您的答案"<<endl;
    getline(cin,answer);
    json data;
    data["name"]=name;
    data["pass"]=pass;
    data["question"]=question;
    data["answer"]=answer;
    data["flag"]=SIGNUP;
    string command=data.dump();
    int ret=asocket.Sendmsg(command);//命令类转换为json格式，再转换为字符串格式，最后由套接字发送
    if(ret==0||ret==-1)
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }

    string uid=asocket.Recvmsg();//收到生成的uid
    if(uid=="close"){
        cout<<"对端已关闭"<<endl;
        exit(0);
    }
    cout<<"您注册的uid为:"<<uid<<endl;

    cout<<"请登录"<<endl;

    return ;
}
string get_uid()
{
    string uid;
    int flag=1,c;
    
    cout<<"您的uid为:"<<endl;
    cin.ignore();
    getline(cin,uid);//获取输入的uid

    for(int c:uid)
    {
        if(!isdigit(c))
        {
            flag=0;
            break;
        }
    }

    while(flag==0)
    {
        cout<<"您输入的uid有非数字,请重新输入:"<<endl;
        cin.ignore();
        getline(cin,uid);

        flag=1;
        for(int c:uid)
        {
            if(!isdigit(c))
            {
                flag=0;
                break;
            }
        }
    }

    return uid;
    
}
int log_in(){
    string uid,pass;
    uid=get_uid();

    //
    cout<<"请输入您的密码:"<<endl;
    getline(cin,pass);
        json data;
    data["pass"]=pass;
    data["flag"]=SIGNUP;
    string command=data.dump();
    
    int ret=asocket.Sendmsg(command);
    if(ret==0||ret==-1){
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }

    string recv=asocket.Recvmsg();//接收返回的结果
    cout<<recv<<endl;
    if(recv=="close"){
        cout<<"服务器已关闭"<<endl;
        exit(0);
       
    }else if(recv=="discorrect"){
        cout<<"密码错误："<<endl;
        return 0;

    }else if(recv=="nonexisent"){
        cout<<"帐号不存在,请注册"<<endl;
        return 0;

    }else if(recv=="ok"){
        system("clear");
        cout<<"登录成功"<<endl;
    pthread_t tid;
    // 传递参数
    struct ThreadParams {
        std::string uid;
        int recv_fd;
    };
    ThreadParams* params = new ThreadParams{uid, asocket.getresvfd()};

    // 创建线程
    pthread_create(&tid, NULL, &recvfunc, static_cast<void*>(params));

    // 分离线程
    pthread_detach(tid);
        return 1;
    }

   
    return 1;
}
void pass_regive(){

}