#include"./client.hpp"
int Main_Menu(){
    while(1){
    string a;
    int ret=0;
    cout<<"-------欢迎来到聊天室-------"<<endl;
    cout<<"---------[1]注册---------"<<endl;
    cout<<"---------[2]登录---------"<<endl;
    cout<<"--------[3]找回密码-------"<<endl;
    cout<<"---------[4]退出---------"<<endl;
    getline(cin,a);
    if(stoi(a)==1)
    sign_up();
    else if(stoi(a)==2)
    ret=log_in();
    else if(stoi(a)==3)
    pass_find(asocket);
    else if(stoi(a)==4)
    exit(0);
    else{
    cout<<"请输入正确的选项";
    continue;
    }
    return ret;
    }
}
void user_menu(){
    cout<<"-------欢迎来到聊天室-------"<<endl;
    cout<<"---------[1]---------"<<endl;
    cout<<"---------[2]---------"<<endl;
    cout<<"--------[3]-------"<<endl;
    cout<<"---------[4]---------"<<endl;
}