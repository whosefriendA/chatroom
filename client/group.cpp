#include"./client.hpp"
void group_create(){
    if(friend_list_get()==0)
    return;
    cout<<"选择一个好友创建群聊"<<endl;
    string frienduid;
    getline(cin,frienduid);
    Message msg(curuid,CREATE_GROUP,{frienduid});
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive_client();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你不能邀请自己进群"<<endl;
        return;
    }else if(recv=="nofind"){
        cout<<"你暂时还没有该好友"<<endl;
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
    string Group;
    while ((Group = asocket.Receive_client()) != "end"){
        err.server_close(Group);
        if (Group == "none"){
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

    Message msg(curuid,groupuid,ADD_GROUP);
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
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
    Message msg(curuid,cur_groupuid,GROUP_IN);
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
    err.server_close(recv);
    if(recv=="none"){
        cout<<"此群不存在"<<endl;
        return;
    }else if(recv=="failure"){
        cout<<"你不在这个群里，请先申请加入"<<endl;
        return;       
    }else if(recv=="success"){
        system("clear");
        Agroup_menu();
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void group_memberlist_get(){
    Message msg(curuid,cur_groupuid,GROUP_MEMBER_LIST);
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string member=asocket.Receive_client();
    while(member!="end"){
        err.server_close(member);
        cout<<member<<endl;
        member=asocket.Receive_client();
    }
    cout<<"The end"<<endl;
    return;
}
void group_apply_list(){
    Message msg(curuid,cur_groupuid,GROUP_APPLY_LIST);
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
    while(recv!="end"){
        err.server_close(recv);
        if(recv=="failure"){
            cout<<"你没有权限"<<endl;
            return;
        }else{
            cout<<recv<<endl;
        }
        recv=asocket.Receive_client();
    }
    cout<<"The end"<<endl;
    return;
}
void group_apply_agree(){
    string agreeuid;
    cout<<"请输入你想同意加群的uid:"<<endl;
    getline(cin,agreeuid);
    Message msg(curuid,cur_groupuid,GROUP_APPLY_AGREE,{agreeuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    
    string agree_recv=asocket.Receive_client();
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
        cout<<agree_recv<<endl;
        cout<<"错误"<<endl;
        return;
    }
}
void group_apply_refuse(){
    string refuseuid;
    cout<<"请输入你想拒绝加群的uid:"<<endl;
    getline(cin,refuseuid);
    Message msg(curuid,cur_groupuid,GROUP_APPLY_REFUSE,{refuseuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string refuse_recv=asocket.Receive_client();
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
    getline(cin,addmanager);

    Message msg(curuid,cur_groupuid,GROUP_MANAGER_SET,{addmanager});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
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
    string unsetmanager;
    cout<<"你想取消的群管理的uid为:"<<endl;
    getline(cin,unsetmanager);

    Message msg(curuid,cur_groupuid,GROUP_MANAGER_UNSET,{unsetmanager});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive_client();
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
    getline(cin,deleteuid);

    Message msg(curuid,cur_groupuid,GROUP_DEL_MEMBER,{deleteuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你没有权限"<<endl;
        return;
    }else if(recv=="failure2"){
        cout<<"你不能踢自己"<<endl;
        return;
    }else if(recv=="success"){
        cout<<"你已成功踢出ta"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
int group_disband(){
    Message msg(curuid,GROUP_DISBAND,{cur_groupuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive_client();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"你没有权限"<<endl;
        return 0;
    }if(recv=="success"){
        system("clear");
        cout<<RED<<"已成功解散此群聊"<<RESET<<endl;
        return 1;
    }else{
        cout<<"错误"<<endl;
        return 0;
    }
}
int group_exit(){
    Message msg(curuid,GROUP_EXIT,{cur_groupuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
    err.server_close(recv);
    if(recv=="failure"){
        cout<<"群主无法退群,可以解散群"<<endl;
        return 0;
    }else if(recv=="success"){
        system("clear");
        cout<<"已成功退出此群聊"<<endl;
        return 1;
    }else{
        cout<<"错误"<<endl;
        return 0;
    }
}
void group_chat(){
    Message msg(curuid,cur_groupuid,GROUP_CHAT);
    int ret=asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive_client();
    err.server_close(recv);
    if(recv=="success"){
        //打印群聊历史聊天记录
        string historymsg;
        while(1){
            historymsg=asocket.Receive_client();
            err.server_close(historymsg);
            if(historymsg=="The end"){
                cout<<RED<<"输入:S发送文件,输入:R接收文件,输入:Q退出聊天"<<RESET<<endl;
                break;
            }else{
                cout<<historymsg<<endl;
            }
        }
    }
    string newmsg;
    while(1){
        getline(cin,newmsg);
        if(newmsg==":S"){
            method.Sendflie_client(asocket,curuid,cur_groupuid,SENDFILE_GROUP);
            continue;
        }else if(newmsg==":R"){
            method.Receiveflie_client(asocket,cur_groupuid,curuid,RECVFILE_GROUP);
            continue;
        }else if(newmsg==":Q"){
                Message msg_exit(curuid,cur_groupuid,GRUOP_CHATEXIT);
                int ret=asocket.Send(msg_exit.S_to_json());
                err.server_close(ret);

                string recv=asocket.Receive_client();
                err.server_close(recv);
                if(recv=="success"){
                    cout<<"成功退出聊天"<<endl;
                    return;
                }else{
                    cout<<"出现错误"<<endl;
                    return;
                }
                break;
        }
        Message msg(curuid,cur_groupuid,GROUP_SENDMSG,{newmsg});
        int ret = asocket.Send(msg.S_to_json());
        err.server_close(ret);

        string recv=asocket.Receive_client();
        err.server_close(recv);
        if(recv=="failure"){
            return;
        }else if(recv=="success"){
            continue;
        }
    }
    return;
}
