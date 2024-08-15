#include"server.hpp"
void group_create(TaskSocket asocket,Message msg){
       // 判断好友列表里有没有该好友
    if (!redis.hexists(msg.uid+"的好友列表",msg.para[0])){
        asocket.Send("nofind");
        return;
    }else if (msg.uid ==msg.para[0]){
        asocket.Send("failure");
        return;
    }
    mutex mtx;
    {unique_lock<mutex> lock(mtx);
    Group_uid+=redis.Scard("群聊集合");
    }
    string groupuid=to_string(Group_uid);

    redis.Sadd("群聊集合",groupuid);
    redis.Hset(groupuid+"的详情","群号",groupuid);
    redis.Hset(groupuid+"群成员列表",msg.uid,"群主");
    redis.Hset(msg.uid+"的群聊列表",groupuid,"群主");

    redis.Rpushvalue(groupuid+"的群聊消息","------");
    string num = redis.Hget(msg.para[0]+"的未读消息","通知类消息");
    redis.Hset(msg.para[0]+"的未读消息","通知类消息",to_string(stoi(num)+1));
    redis.Rpushvalue(msg.para[0]+"的通知消息",msg.uid+"邀请你加入群聊"+groupuid);

    redis.Hset(groupuid+"群成员列表",msg.para[0],"普通成员");
    redis.Hset(msg.para[0]+"的群聊列表",groupuid,"普通成员");
    if (online_users.find(msg.para[0]) != online_users.end()){
        string friend_fd = redis.Hget(msg.para[0],"实时socket");
        TaskSocket friendsocket(stoi(friend_fd));
        friendsocket.Send(BLUE+msg.uid+"邀请你加入群聊"+RESET);
    }
    asocket.Send(groupuid);
    return;
}
void group_add(TaskSocket asocket,Message msg){
    if (redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("already");
        return;
    }else if (!redis.Sismember("群聊集合",msg.recuid)){
        asocket.Send("none");
        return;
    }else if (redis.hexists(msg.recuid+"的申请加群列表",msg.uid)){
        asocket.Send("again");
        return;
    }else{
        string mesg = "申请加入群聊";
        redis.Hset(msg.recuid+"的申请加群列表",msg.uid,mesg);
        // cout<<"whathappend"<<endl;
        string recv=redis.Hget(msg.recuid+"的申请加群列表",msg.uid);
        // cout<<recv<<endl;
        vector<string> memberlist = redis.Hgetall(msg.recuid,"群成员列表");
        for (const string &memberid : memberlist){
            if (redis.Hget(msg.recuid+"群成员列表",memberid) != "普通成员"){
                string apply = msg.uid+"申请加入群聊"+msg.recuid;
                string num = redis.Hget(memberid+"的未读消息","群聊消息");
                redis.Hset(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.Rpushvalue(memberid+"群聊消息",apply);
                if(redis.Sismember("在线用户列表",memberid)){
                    string member_fd = redis.Hget(memberid,"实时socket");
                    TaskSocket membersocket(stoi(member_fd));
                    membersocket.Send(BLUE+apply+RESET);
                }
            }
        }
        asocket.Send("success");
        return;
    }
}
void group_in(TaskSocket asocket,Message msg){
    if (!redis.Sismember("群聊集合",msg.recuid)){
        asocket.Send("none");
        return;
    }else if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("failure");
        return;
    }else if (redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("success");
        return;
    }
}
void group_list_get(TaskSocket asocket,Message msg){
    if (!redis.exists(msg.uid+"的群聊列表")){
        asocket.Send("none");
        return;
    }
    vector<string> grouplist = redis.Hgetall(msg.uid,"的群聊列表");
    for (const string &groupid : grouplist){
        asocket.Send(groupid);
    }
    asocket.Send("end");
}
void group_memberlist_get(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("failure");
        return;
    }
    vector<string> memberlist = redis.Hgetall(msg.recuid,"群成员列表");
    for (const string &memberid : memberlist){
        asocket.Send(memberid);
    }
    asocket.Send("end");
}
void group_apply_list(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
    if (redis.Hget(msg.recuid+"群成员列表",msg.uid) != "普通成员"){
        vector<string> memberlist = redis.Hgetall(msg.recuid,"的申请加群列表");
        for (const string &memberid : memberlist){
            asocket.Send(memberid);
        }
        asocket.Send("end");
    }else{
        asocket.Send("failure");
    }
}
void group_apply_agree(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
    if (!redis.hexists(msg.recuid+"的申请加群列表",msg.para[0])){
        asocket.Send("notfind");
        return;
    }else{
        redis.HValueremove(msg.recuid+"的申请加群列表",msg.para[0]);
        redis.Hset(msg.recuid+"群成员列表",msg.para[0],"普通成员");//设置
        redis.Hset(msg.para[0]+"的群聊列表",msg.recuid,"普通成员");
        vector<string> memberlist = redis.Hgetall(msg.para[0],"群成员列表");

        for (const string &memberid : memberlist){
            if (redis.Hget(msg.para[0]+ "群成员列表",memberid) != "普通成员" && msg.uid != memberid){
                string apply = msg.uid+"同意了"+msg.para[0]+"的加群申请";
                string num = redis.Hget(memberid+"的未读消息","群聊消息");
                redis.Hset(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.Rpushvalue(memberid+"群聊消息",apply);

                if (online_users.find(memberid) != online_users.end()){
                    string member_fd = redis.Hget(memberid,"实时socket");
                    TaskSocket membersocket(stoi(member_fd));
                    membersocket.Send(BLUE+apply+RESET);
                }
            }
        }
    }
    asocket.Send("success");
    if (online_users.find(msg.para[0]) != online_users.end()){
        string member_fd = redis.Hget(msg.para[0],"实时socket");
        TaskSocket membersocket(stoi(member_fd));
        membersocket.Send(BLUE+msg.recuid+":你的加群申请已被通过"+RESET);
    }
}
void group_apply_refuse(TaskSocket asocket,Message msg){
     if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
    if (!redis.hexists(msg.recuid+"的申请加群列表",msg.para[0])){
        asocket.Send("notfind");
        return;
    }else{
        redis.HValueremove(msg.recuid+"的申请加群列表",msg.para[0]);
        vector<string> memberlist = redis.Hgetall(msg.para[0],"群成员列表");

        for (const string &memberid : memberlist){
            if (redis.Hget(msg.para[0]+ "群成员列表",memberid) != "普通成员" && msg.uid != memberid){
                string apply = msg.uid+"拒绝了"+msg.para[0]+"的加群申请";
                string num = redis.Hget(memberid+"的未读消息","群聊消息");
                redis.Hset(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.Rpushvalue(memberid+"群聊消息",apply);

                if (online_users.find(memberid) != online_users.end()){
                    string member_fd = redis.Hget(memberid,"实时socket");
                    TaskSocket membersocket(stoi(member_fd));
                    membersocket.Send(BLUE+apply+RESET);
                }
            }
        }
    }
    asocket.Send("success");
    if (online_users.find(msg.para[0]) != online_users.end()){
        string member_fd = redis.Hget(msg.para[0],"实时socket");
        TaskSocket membersocket(stoi(member_fd));
        membersocket.Send(BLUE+msg.recuid+":你的加群申请已被拒绝"+RESET);
    }
}
void group_manager_set(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
     else if(!redis.hexists(msg.recuid+"群成员列表",msg.para[0])){
        asocket.Send("none");
        return ;
    }
    if (redis.Hget(msg.recuid+"群成员列表",msg.uid) != "群主"){
        asocket.Send("failure");
        return;
    }else if(msg.uid==msg.para[0]){
        asocket.Send("whatthef");
        return;
    }
    else if (redis.Hget(msg.recuid+"群成员列表",msg.para[0]) == "群管理员"){
        asocket.Send("again");
        return;
    }
    redis.Hset(msg.recuid+"群成员列表",msg.para[0],"群管理员");
    if (online_users.find(msg.para[0]) != online_users.end()){
        string member_fd = redis.Hget(msg.para[0],"实时socket");
        TaskSocket membersocket(stoi(member_fd));
        membersocket.Send(BLUE+msg.uid+"将你添加为群聊"+msg.recuid+"的群管理员"+RESET);
    }
    asocket.Send("success");
}
void group_manager_unset(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
    else if(!redis.hexists(msg.recuid+"群成员列表",msg.para[0])){
        asocket.Send("none");
        return ;
    }
    if (redis.Hget(msg.recuid+"群成员列表",msg.uid) != "群主"){
        asocket.Send("failure");
        return;
    }else if (redis.Hget(msg.recuid+"群成员列表",msg.para[0]) != "群管理员"){
        asocket.Send("again");
        return;
    }
    redis.Hset(msg.recuid+"群成员列表",msg.para[0],"普通成员");
    if (online_users.find(msg.para[0]) != online_users.end()){
        string member_fd = redis.Hget(msg.para[0],"实时socket");
        TaskSocket membersocket(stoi(member_fd));
        membersocket.Send(BLUE+msg.uid+"取消了你的"+msg.recuid+"的群管理员身份"+RESET);
    }
    asocket.Send("success");
}
void group_delmember(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }else if(!redis.hexists(msg.recuid+"群成员列表",msg.para[0])){
        asocket.Send("none");
        return ;
    }
    else if (redis.Hget(msg.recuid+"群成员列表",msg.uid)=="普通成员"){
        asocket.Send("failure");
        return;
    }else if (redis.Hget(msg.recuid+"群成员列表",msg.uid)!="普通成员" && msg.uid == msg.para[0]){
        asocket.Send("failure2");
        return;
    }else if ((redis.Hget(msg.recuid+"群成员列表",msg.uid)!="普通成员") && (redis.Hget(msg.recuid+"群成员列表",msg.para[0]) == "群主")){
        asocket.Send("failure");
        return;
    }else if ((redis.Hget(msg.recuid+"群成员列表",msg.uid) == "群管理员") && (redis.Hget(msg.recuid+"群成员列表",msg.para[0]) == "群管理员")){
        asocket.Send("failure");
        return;
    }
    redis.HValueremove(msg.para[0]+"的群聊列表",msg.recuid);
    redis.HValueremove(msg.recuid+"群成员列表",msg.para[0]);

    vector<string> memberlist = redis.Hgetall(msg.para[0],"群成员列表");
    for (const string &memberid : memberlist){
        if (redis.Hget(msg.recuid+"群成员列表",memberid) != "普通成员"){
            string apply = msg.uid+"将"+msg.para[0]+"踢出群聊"+msg.recuid;
            string num = redis.Hget(memberid+"的未读消息","群聊消息");
            redis.Hset(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
            redis.Rpushvalue(memberid+"群聊消息",apply);

            if (online_users.find(memberid) != online_users.end()){
                string member_fd = redis.Hget(memberid,"实时socket");
                TaskSocket membersocket(stoi(member_fd));
                membersocket.Send(BLUE+apply+RESET);
            }
        }
    }
    asocket.Send("success");
    return;
}
void group_disband(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.para[0]+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
    if (redis.Hget(msg.para[0]+"群成员列表", msg.uid) != "群主"){
        asocket.Send("failure");
        return;
    }
    // cout<<1<<endl;
    redis.Valueremove("群聊集合", msg.para[0]);
    // cout<<2<<endl;
    vector<string> memberlist = redis.Hgetall(msg.para[0], "群成员列表");
    for (const string &memberid : memberlist){
        redis.HValueremove(memberid+"的群聊列表", msg.para[0]);
        string apply = msg.uid+"解散了"+msg.para[0];
        string num = redis.Hget(memberid+"的未读消息", "群聊消息");
        redis.Hset(memberid+"的未读消息", "群聊消息", to_string(stoi(num)+1));
        redis.Rpushvalue(memberid+"群聊消息", apply);

        if ((online_users.find(memberid) != online_users.end()) && redis.Hget(msg.para[0]+"群成员列表", memberid) != "群主"){
            string member_fd = redis.Hget(memberid, "实时socket");
            TaskSocket membersocket(stoi(member_fd));
            membersocket.Send(BLUE+apply+RESET);
        }
    }
    // cout<<3<<endl;
    redis.keyremove(msg.para[0]+"的信息");
    redis.keyremove(msg.para[0]+"的群聊消息");
    redis.keyremove(msg.para[0]+"群成员列表");

    asocket.Send("success");
}
void group_exit(TaskSocket asocket,Message msg){
    if (redis.Hget(msg.para[0]+"群成员列表", msg.uid) == "群主"){
        asocket.Send("failure");
        return;
    }
    redis.HValueremove(msg.uid+"的群聊列表", msg.para[0]);
    redis.HValueremove(msg.para[0]+"群成员列表", msg.uid);
    asocket.Send("success");
}
void group_chat(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.recuid+"群成员列表",msg.uid)){
        asocket.Send("notingroup");
        return;
    }
    else {asocket.Send("success");
    }
    vector<string> history = redis.Lrangeall(msg.recuid+"的群聊消息");
    for (const string &msg : history){
        asocket.Send(msg);
    }
    redis.Hset(msg.uid, "聊天对象", msg.recuid);
    asocket.Send("The end");
}
void group_chatexit(TaskSocket asocket,Message msg){
    redis.Hset(msg.uid, "聊天对象", "0");
    asocket.Send("success");
    return;
}
void group_sendmsg(TaskSocket asocket,Message msg){
    string newmsg = msg.uid+":"+msg.para[0];
    redis.Rpushvalue(msg.recuid+"的群聊消息", newmsg);

    string my_recvfd = redis.Hget(msg.uid, "实时socket");
    TaskSocket my_socket(stoi(my_recvfd));
    my_socket.Send(GREEN+ newmsg+RESET);

    string uid = msg.uid;
    string msg1 = uid+":"+msg.para[0];

    vector<string> memberlist = redis.Hgetall(msg.recuid, "群成员列表");

    for (const string &memberid : memberlist){
        if ((online_users.find(memberid) == online_users.end()) && memberid != uid){
            string apply = msg.recuid+"群聊中有人发来了一条消息";
            string num = redis.Hget(memberid+"的未读消息", "群聊消息");
            redis.Hset(memberid+"的未读消息", "群聊消息", to_string(stoi(num)+1));
            redis.Rpushvalue(memberid+"群聊消息", apply);
        }else if ((online_users.find(memberid) != online_users.end()) && (redis.Hget(memberid, "聊天对象") == msg.recuid)){
            if (memberid != uid){
                string gr_recvfd = redis.Hget(memberid, "实时socket");
                TaskSocket gr_socket(stoi(gr_recvfd));
                gr_socket.Send(WIDEWHITE+msg1+RESET);
            }
        }else if ((online_users.find(memberid) != online_users.end()) && memberid != uid){
            string apply = msg.recuid+"群聊中有人发来了一条消息";
            string gr_recvfd = redis.Hget(memberid, "实时socket");
            TaskSocket gr_socket(stoi(gr_recvfd));
            gr_socket.Send(RED+apply+RESET);
        }
    }
    asocket.Send("success");
    return;
}