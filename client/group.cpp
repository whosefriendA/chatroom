#include"./client.hpp"
void group_create(){
    friend_list_get();
    cout<<"选择一个好友创建群聊"<<endl;
    string frienduid;
    getline(cin,frienduid);
    Message msg(curuid,CREATE_GROUP,{frienduid});
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你不能邀请自己进群"<<endl;
        return;
    }else if(recv=="nofind"){
        cout<<"您暂时还没有该好友"<<endl;
        return;
    }else{
    cout<<"群聊创建成功,群号为:"<<recv<<endl;
    }
    return;
}
void group_list_get(){
    Message msg(curuid,GROUP_LIST);
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    system("clear");
    string Group;
    while ((Group = asocket.Receive()) != "end"){
        err.server_close(Group);
        if (Group == "none"){
            system("clear");
            cout << "你当前还没有加入群聊" << endl;
            return;
        }else{
            cout << Group << endl;
        }
    }
    cout << "The end" << endl;
    return;
}
void group_add(){
    string groupuid;
    cout<<"输入你想加入的群聊的uid:"<<endl;
    getline(cin,groupuid);

    Message msg(curuid,ADD_GROUP,{groupuid});
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="already"){
        cout<<"你已在该群聊中"<<endl;
        return;
    }else if(recv=="none"){
        cout<<"此群不存在"<<endl;
        return;
    }else if(recv=="success"){
        cout<<"申请已发送"<<endl;
        return;
    }else if(recv=="again"){
        cout<<"已发送过申请"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_in(){
    cout<<"输入你要进入的群聊uid:"<<endl;
    getline(cin,cur_groupuid);
    Message msg(curuid,GROUP_IN,{cur_groupuid});
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="none"){
        cout<<"此群不存在"<<endl;
        return;
    }else if(recv=="failure"){
        cout<<"你不在这个群里，请先申请加入"<<endl;
        return;       
    }else if(recv=="success"){
        Agroup_menu();
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_memberlist_get(){
    Message msg(curuid,GROUP_MEMBER_LIST,{cur_groupuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string member=asocket.Receive();
    while(member!="end"){
        err.server_close(member);
        cout<<member<<endl;
        member=asocket.Receive();
    }
    cout<<"The end"<<endl;
    return;
}
void group_apply_list(){
    Message msg(curuid,GROUP_APPLY_LIST,{cur_groupuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    while(recv!="end"){
        err.server_close(recv);
        if(recv=="failure"){
            cout<<"你没有权限"<<endl;
            return;
        }else{
            cout<<recv<<endl;
        }
        recv=asocket.Receive();
    }
    cout<<"The end"<<endl;
    return;
}
void group_apply_agree(){
    string agreeuid;
    cout<<"请输入你想同意加群的uid:"<<endl;
    cin>>agreeuid;
    Message msg(curuid,cur_groupuid,GROUP_APPLY_AGREE,{agreeuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string agree_recv=asocket.Receive();
    err.server_close(agree_recv);
    if(agree_recv=="failure"){
    cout<<"你没有权限"<<endl;
    }else if(agree_recv=="notfind"){
        cout<<"此人未申请加群"<<endl;
        return;
    }else if(agree_recv=="success"){
        cout<<"已通过该用户的申请"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_apply_refuse(){
    string refuseuid;
    cout<<"请输入你想拒绝加群的uid:"<<endl;
    cin>>refuseuid;
    Message msg(curuid,cur_groupuid,GROUP_APPLY_REFUSE,{refuseuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string refuse_recv=asocket.Receive();
    err.server_close(refuse_recv);
    if(refuse_recv=="failure"){
        cout<<"你没有权限"<<endl;
        return;
    }else if(refuse_recv=="notfind"){
        cout<<"此人未申请加群"<<endl;
        return;
    }else if(refuse_recv=="success"){
        cout<<"已拒绝该用户的申请"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_manager_set(){
    group_memberlist_get();
    string addmanager;
    cout<<"你想添加为管理员的uid为:"<<endl;
    cin>>addmanager;

    Message msg(curuid,cur_groupuid,GROUP_MANAGER_SET,{addmanager});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你没有权限"<<endl;
        return;
    }else if(recv=="again"){
        cout<<"ta已经是群管理员,不能再添加"<<endl;
        return;
    }else if(recv=="success")
    {
        cout<<"已成功添加"<<addmanager<<"为群管理员"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_manager_unset(){
    group_memberlist_get();
    string deletemanager;
    cout<<"你想取消的群管理的uid为:"<<endl;
    cin>>deletemanager;

    Message msg(curuid,cur_groupuid,GROUP_MANAGER_UNSET,{deletemanager});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你没有权限"<<endl;
        return;
    }else if(recv=="again"){
        cout<<"ta不是管理员"<<endl;
        return;
    }else if(recv=="success"){
        cout<<"已成功取消ta管理员身份"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}
void group_delmember(){
    group_memberlist_get();
    string deleteuid;
    cout<<"你想踢出的群成员的uid为:"<<endl;
    cin>>deleteuid;

    Message msg(curuid,cur_groupuid,GROUP_DEL_MEMBER,{deleteuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你不能踢自己"<<endl;
        return;
    }else if(recv=="failure2"){
        cout<<"你没有权限"<<endl;
        return;
    }else if(recv=="ok"){
        cout<<"你已成功踢出ta"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_disband(){
    Message msg(curuid,GROUP_DISBAND,{cur_groupuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你没有权限"<<endl;
        return;
    }if(recv=="success"){
        system("clear");
        cout<<RED<<"已成功解散此群聊"<<RESET<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_exit(){
    
}