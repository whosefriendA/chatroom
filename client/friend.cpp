#include"./client.hpp"
void friend_list_get(){
    Message msg(curuid,FRIEND_LIST); // 展示好友列表
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string Friend;
    while ((Friend = asocket.Receive()) != "end"){
        if (Friend == "none"){
            cout << "你当前还没有好友" << endl;
            return;
        }else if (Friend == "close"){
            cout << "服务器端已关闭" << endl;
            exit(0);
        }else{
            cout << Friend << endl;
        }
    }
    cout << "黄色表示好友在线" << endl;
    return;
}
void freind_add(){
    string recv_uid;//被添加好友的uid
    string option;//发送的验证消息
    cout<<"您想添加的好友的uid为:"<<endl; 
    cin>>recv_uid;
    cout<<"您想发送的验证消息为"<<endl;
    cin.ignore();
    getline(cin,option);

    Message command1(curuid,recv_uid,ADD_FRIEND,{option});
    int ret = asocket.Send(command1.S_to_json());
    err.server_close(ret);
    string recv = asocket.Receive();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "好友添加申请已发送,等待对方通过" << endl;
        return ;
    }else if (recv == "exist"){
        system("clear");
        cout << "该用户已是好友,不能重复添加" << endl;
        return ;
    }else if(recv=="handle"){
        system("clear");
        cout<<"已向该好友发送过好友申请,请等待回复"<<endl;
        return ;
    }else if (recv == "apply"){
        system("clear");
        cout << "系统消息中存在对方发送的好友申请,请先回复" << endl;
        return ;
    }else if(recv=="none"){
        cout << "该用户不存在" << endl;
        return ;
    }else{
        cout<<"其他错误"<<endl;
        return ;
    }
}
void friend_del(){
    friend_list_get();
    string deluid;
    cout<<"请输入想删除的好友的uid:"<<endl;
    cin>>deluid;

    Message msg(curuid,DELETE_FRIEND,{deluid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv = asocket.Receive();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "已成功删除该好友" << endl;
        return ;
    }else if (recv == "none"){
        cout << "未找到该好友" << endl;
        return ;
    }else{
        cout<<"其他错误"<<endl;
        return ;
    }
}
void friend_apply_agree(){
    string agreeuid;//同意的uid
    cout<<"想同意加好友申请的好友uid为:"<<endl;
    cin>>agreeuid;

    Message msg(curuid,AGREE_ADDFRIEND,{agreeuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv = asocket.Receive();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "已通过" << agreeuid << "的好友申请" << endl;
        return;
    }else if(recv=="notfind"){
        cout<<"不存在该用户的好友申请"<<endl;
        return ;
    }else{
        cout << "错误" << endl;
        return;
    }
}
void friend_apply_refuse(){
    friend_list_get();
    string refuseuid;//拒绝的uid
    cout<<"想拒绝加好友申请的uid为:"<<endl;
    cin>>refuseuid;

    Message msg(curuid,REFUSE_ADDFRIEND,{refuseuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="notfind"){
        cout<<"不存在ta的好友申请"<<endl;
        return;
    }else if(recv=="ok"){
        system("clear");
        cout<<"已拒绝"<<refuseuid<<"的申请"<<endl;
        return;
    }else{
        cout<<recv<<endl;
        cout<<"错误"<<endl;
        return;
    }
}
void friend_block(){
    friend_list_get();
    string blockuid;//屏蔽好友
    cout<<"想屏蔽的好友的uid为:"<<endl;
    cin>>blockuid;

    Message msg(curuid,BLOCK_FRIEND,{blockuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="success"){
        cout<<"已屏蔽该好友"<<endl;
        return;
    }else if(recv=="none"){
        cout<<"未找到该好友"<<endl;
        return;
    }else if(recv=="handled"){
        system("clear");
        cout<<"已屏蔽过该好友,重复操作"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void friend_restore(){
    string restoreuid;//恢复好友会话
    cout<<"想恢复聊天的好友uid为:"<<endl;
    cin>>restoreuid;

    Message msg(curuid,RESTORE_FRIEND,{restoreuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="success"){
        cout<<"已恢复与ta聊天"<<endl;
        return;
    }else if(recv=="none"){
        cout<<"未找到该好友"<<endl;
        return;
    }else if(recv=="no"){
        system("clear");
        cout<<"该好友未被屏蔽"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void friend_chat()
{
    friend_list_get();
    string recvuid;
    cout<<"想聊天好友的uid为:"<<endl;
    cin>>recvuid;

    Message msg1(curuid,recvuid,CHAT_WITH_FRIEND);
    int ret = asocket.Send(msg1.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="notfind"){
        cout<<"ta不是你的好友"<<endl;
        return;
    }else if(recv=="success"){
        //打印历史聊天记录
        string historymsg;
        while(1){
            historymsg=asocket.Receive();
            err.server_close(historymsg);
            if(historymsg=="The end"){
                cout<<"上面为历史聊天记录，开始新的聊天吧"<<endl;
                cout<<RED<<"输入:S发送文件,输入:R接收文件,输入:Q退出聊天"<<RESET<<endl;
                break;
            }else{
                cout<<historymsg<<endl;
            }
        }
        cin.ignore();
        string curmsg;
        while(1){
            getline(cin,curmsg);
            if(curmsg==":Q"){
                //退出聊天
                Message msg_exit(curuid,recvuid,EXIT_CHAT);
                int ret=asocket.Send(msg_exit.S_to_json());
                err.server_close(ret);

                string recv=asocket.Receive();
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
            //发送文件
            if(curmsg==":S"){
                method.Sendflie_client(asocket,curuid,recvuid,SENDFILE);
                continue;
            }
            //接收文件
            if(curmsg==":R"){
                method.Receiveflie_client(asocket,curuid,recvuid,RECVFILE);
                continue;
            }
            //发消息
            Message msg(curuid,recvuid,SEND_MSG,{curmsg});
            int ret = asocket.Send(msg.S_to_json());
            err.server_close(ret);

            string recv=asocket.Receive();
            err.server_close(recv);
            if(recv=="failure"){
                return;
            }else if(recv=="success"){
                continue;
            }
        }
    }
    return ;

}