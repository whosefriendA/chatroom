#include"./client.hpp"
void friend_list_get(){
    string uid,friend_list;
    uid=get_uid();
    json data;
    data["UID"]=uid;
    data["flag"]=FRIEND_LIST;
    string command=data.dump();
    int ret=asocket.Sendmsg(command);
    err.server_close(ret);
    string recv=asocket.Recvmsg();
    err.server_close(recv);
}
void freind_add(){
    string recv_uid;//接收者的uid,或者说要添加好友的uid
    string option;//发送的验证消息
    cout<<"您想添加的好友的uid为:"<<endl; 
    cin>>recv_uid;
    cout<<"您想发送的验证消息为"<<endl;
    cin.ignore();
    getline(cin,option);

    Message command1(curuid,"",recv_uid,ADD_FRIEND,{option});//将发送者和接收者的uid以及发送者发送的验证消息打包
    int ret = asocket.Sendmsg(command1.S_to_json());
    err.server_close(ret);
    string recv = asocket.Recvmsg();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "好友添加申请已发送,等待对方通过" << endl;
        return ;
    }
    else if (recv == "exist"){
        system("clear");
        cout << "该用户已经是您的好友,无需反复添加" << endl;
        return ;
    }else if(recv=="handle"){
        system("clear");
        cout<<"您已经向该好友发送过好友申请,请等待回复"<<endl;
        return ;
    }
    else if (recv == "apply"){
        system("clear");
        cout << "您的系统消息中存在对方发送的好友申请,请先回复" << endl;
        return ;
    }
    else if(recv=="none"){
        cout << "该用户不存在" << endl;
        return ;
    }else{
        cout<<"其他错误"<<endl;
        return ;
    }
}
void friend_del(){
    friend_list_get();
    string deluid;//想删除的好友的uid
    cout<<"请输入您想删除的好友的uid:"<<endl;
    cin>>deluid;

    Message command1(curuid,"","",DELETE_FRIEND,{deluid});
    int ret = asocket.Sendmsg(command1.S_to_json());
    err.server_close(ret);

    string recv = asocket.Recvmsg();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "您已成功删除该好友" << endl;
        return ;
    }
    else if (recv == "none"){
        //system("clear");
        cout << "未找到该好友" << endl;
        return ;
    }else{
        cout<<"其他错误"<<endl;
        return ;
    }
}
void friend_apply(){
    
}