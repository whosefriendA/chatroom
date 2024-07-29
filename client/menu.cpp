#include"client.hpp"
#include"sign.cpp"
int Main_Menu(){
    while(1){
    int a;
    int ret;
    cout<<"-------欢迎来到聊天室-------"<<endl;
    cout<<"---------[1]注册---------"<<endl;
    cout<<"---------[2]登录---------"<<endl;
    cout<<"-------[3]找回密码-------"<<endl;
    cout<<"---------[4]退出---------"<<endl;
    if(a==1)
    sign_up();
    else if(a==2)
    ret=log_in();
    else if(a==3)
    pass_regive();
    else if(a==4)
    exit(0);
    else{
    cout<<"请输入正确的选项";
    continue;
    }
    return ret;
    }
}