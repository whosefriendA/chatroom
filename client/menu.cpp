#include"./client.hpp"
int Main_menu(){
    while(1){
    string a;
    int ret=0;
    cout<<"-------欢迎来到聊天室------"<<endl;
    cout<<"---------[1]注册---------"<<endl;
    cout<<"---------[2]登录---------"<<endl;
    cout<<"--------[3]找回密码-------"<<endl;
    cout<<"---------[4]退出---------"<<endl;
    getline(cin,a);
    if(stoi(a)==1){
    sign_up();
    continue;
    }
    else if(stoi(a)==2)
    ret=log_in();
    else if(stoi(a)==3)
    pass_find();
    else if(stoi(a)==4)
    exit(0);
    else{
    cout<<"请输入正确的选项";
    continue;
    }
    return ret;
    }
}
void User_menu(){
    string a;
    while(1){
    cout<<"---------个人空间---------"<<endl;
    cout<<"---------[1]好友---------"<<endl;
    cout<<"---------[2]群聊---------"<<endl;
    cout<<"---------[3]注销---------"<<endl;
    cout<<"---------[4]退出---------"<<endl;
    getline(cin,a);
    if(stoi(a)==1)
    Friend_menu();  
    else if(stoi(a)==2)
;
    else if(stoi(a)==3)
;
    else if(stoi(a)==4)
    exit(0);
    else{
    cout<<"请输入正确的选项";
    continue;
    }
    }
}
void Friend_menu(){
    string a;
    while(1){
    cout<<"------------好友界面-----------"<<endl;
    cout<<"--------[1]查看好友列表--------"<<endl;
    cout<<"---------[2]添加好友----------"<<endl;
    cout<<"---------[3]删除好友----------"<<endl;
    cout<<"---------[4]好友申请----------"<<endl;
    cout<<"--------[5]屏蔽好友消息--------"<<endl;
    cout<<"-----------[6]私聊-----------"<<endl;
    cout<<"-----------[7]退出-----------"<<endl;
    getline(cin,a);
    if(stoi(a)==1)
    friend_list_get();
    else if(stoi(a)==2)
    freind_add();
    else if(stoi(a)==3)
    friend_del();
    else if(stoi(a)==4)
    friend_apply();
    else if(stoi(a)==5)
;
    else if(stoi(a)==6)
;
    else if(stoi(a)==7)
    exit(0);
    else{
    cout<<"请输入正确的选项";
    continue;
    }
    }
}