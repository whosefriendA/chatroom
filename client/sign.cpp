#include"client.hpp"
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

        cout<<"请确认您的密码:"<<endl;
        getline(cin,pass);
        if(pass!=pass2)
        {
            system("clear");
            cout<<"两次密码不一致,请重新操作"<<endl;
        }else{
            break;
        }
        
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
int log_in(){

}
void pass_regive(){

}