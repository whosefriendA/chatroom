#include"./client.hpp"
int Main_menu(){
    while(1){
    string a;
    int ret=0;
    cout<<"--------"<<WIDEWHITE<<"欢迎来到聊天室"<<RESET<<"-------"<<endl;
    cout<<"         [1]注册       "<<endl;
    cout<<"         [2]登录       "<<endl;
    cout<<"         [3]找回密码        "<<endl;
    cout<<"         [4]退出      "<<endl;
    cout<<"-----------------------------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
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
    return ret;
    }catch (const std::invalid_argument& e) {
    cout << "请输入正确的选项" << endl;
    continue;
    }
    }
}
void User_menu(){
    string a;
    while(1){
    cout<<"   ----"<<GREEN<<curname<<RESET<<"的"<<WIDEWHITE<<"个人空间"<<RESET<<"----"<<endl;
    cout<<"         [1]好友         "<<endl;
    cout<<"         [2]群聊         "<<endl;
    cout<<"         [3]未读消息       "<<endl;
    cout<<"         [4]注销        "<<endl;
    cout<<"         [5]退出         "<<endl;
    cout<<"-------------------------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    Friend_menu();  
    else if(choice==2)
    Group_menu();
    else if(choice==3)
    Unreadnotice();
    else if(choice==4)
;
    else if(choice==5)
    exit(0);
    }catch (const std::invalid_argument& e) {
    cout << "请输入正确的选项" << endl;
    continue;
    }
    }
}
void Friend_menu(){
    string a;
    while(1){
    cout<<"-----------"<<WIDEWHITE<<"好友界面"<<RESET<<"----------"<<endl;
    cout<<"        [1]查看好友列表"<<endl;
    cout<<"         [2]添加好友"<<endl;
    cout<<"         [3]删除好友"<<endl;
    cout<<"         [4]好友申请"<<endl;
    cout<<"        [5]屏蔽好友消息"<<endl;
    cout<<"          [6]私聊"<<endl;
    cout<<"          [7]返回"<<endl;
    cout<<"-----------------------------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    friend_list_get();
    else if(choice==2)
    freind_add();
    else if(choice==3)
    friend_del();
    else if(choice==4)
    Friend_apply();
    else if(choice==5)
    Friend_block();
    else if(choice==6)
    friend_chat();
    else if(choice==7)
    return;
    }catch (const std::invalid_argument& e) {
    cout << "请输入正确的选项" << endl;
    continue;
   }
  }
}
void Friend_apply(){
    string a;
    while(1){
    cout<<"------------好友申请----------"<<endl;
    cout<<"--------[1]同意好友申请--------"<<endl;
    cout<<"--------[2]拒绝好友申请--------"<<endl;
    cout<<"-----------[3]返回-----------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    friend_apply_agree();
    else if(choice==2)
    friend_apply_refuse();
    else if(choice==3)
    return;
  }catch (const std::invalid_argument& e) {
    cout << "请输入正确的选项" << endl;
    continue;
}
}
}

void Friend_block(){
    string a;
    while(1){
    cout<<"--------[1]屏蔽好友消息--------"<<endl;
    cout<<"--------[2]恢复好友消息--------"<<endl;
    cout<<"-----------[3]返回-----------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    friend_block();
    else if(choice==2)
    friend_restore();
    else if(choice==3)
    return;
    }catch(const std::invalid_argument& e){
      cout << "请输入正确的选项" << endl;
      continue;
    }
  }
}
void Group_menu(){
    string a;
    while(1){
    cout<<"-----------"<<WIDEWHITE<<"群聊界面"<<RESET<<"----------"<<endl;
    cout<<"        [1]查看群聊列表"<<endl;
    cout<<"         [2]创造群聊"<<endl;
    cout<<"         [3]加入群聊"<<endl;
    cout<<"         [4]进入群聊"<<endl;
    cout<<"          [5]返回"<<endl;
    cout<<"-----------------------------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    group_list_get();
    else if(choice==2)
    group_create();
    else if(choice==3)
    group_add();
    else if(choice==4)
    group_in();
    else if(choice==5)
    return;
    }catch (const std::invalid_argument& e) {
    cout << "请输入正确的选项" << endl;
    continue;
   }
  }
}
void Agroup_menu(){
    string a;
    while(1){
    cout<<"-----------"<<WIDEWHITE<<"群聊："<<RESET<<GREEN<<cur_groupuid<<RESET<<"----------"<<endl;
    cout<<"         [1]查看群聊成员"<<endl;
    cout<<"         [2]添加管理员（群主）"<<endl;
    cout<<"         [3]取消管理员（群主）"<<endl;
    cout<<"         [4]处理入群申请(管理员及以上权限)"<<endl;
    cout<<"         [5]踢人（管理员及以上权限）"<<endl;
    cout<<"         [6]聊天"<<endl;
    cout<<"         [7]退群"<<endl;
    cout<<"         [8]解散群聊（群主）"<<endl;
    cout<<"         [9]返回"<<endl;
    cout<<"-----------------------------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    group_memberlist_get();
    else if(choice==2)
    group_manager_set();
    else if(choice==3)
    group_manager_unset();
    else if(choice==4)
    Group_apply();
    else if(choice==5)
    group_delmember();
    else if(choice==6)
    group_chat();
    else if(choice==7){
    if(group_exit())
    return;
    }
    else if(choice==8){
    if(group_disband())
    return;
    }
    else if(choice==9)
    return;
    }catch (const std::invalid_argument& e) {
    cout << "请输入正确的选项" << endl;
    continue;
   }
  }
}
void Group_apply(){
    string a;
    while(1){
    cout<<"--------[1]查看申请列表--------"<<endl;
    cout<<"--------[2]通过入群申请--------"<<endl;
    cout<<"--------[3]拒绝入群申请--------"<<endl;
    cout<<"----------[4]返回------------"<<endl;
    getline(cin,a);
    try{
    int choice=stoi(a);
    if(choice==1)
    group_apply_list();
    else if(choice==2)
    group_apply_agree();
    else if(choice==3)
    group_apply_refuse();
    else if(choice==4)
    return;
    }catch(const std::invalid_argument& e){
      cout << "请输入正确的选项" << endl;
      continue;
    }
  }
}