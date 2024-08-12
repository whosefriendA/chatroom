#include"server.hpp"
void friend_list_get(TaskSocket asocket,Message msg){
    if (!redis.exists(msg.uid+"的好友列表")){
        asocket.Send("none");
        return;
    }
    vector<string> friendList = redis.Hgetall(msg.uid,"的好友列表");
    for (const string &friendID : friendList){
        if (!redis.Sismember(msg.uid+"的屏蔽列表",friendID)){
            // cout<<"下面是在线用户列表"<<endl;
            // vector<string> members=redis.Sgetall(online_users);
            // for(const string&member :members){
            //     cout<<member<<endl;
            // }
            if (online_users.find(friendID) != online_users.end()){
                asocket.Send(YELLOW+friendID+RESET);
            }else{
                asocket.Send(friendID);
            }
        }
    }
    asocket.Send("end");
}
void friend_add(TaskSocket asocket,Message msg){
    if (!redis.Sismember("uid集合",msg.recuid)){
        asocket.Send("none"); // 该用户不存在
        return;
    }
    int num = redis.Sgetcount(msg.uid,"的好友列表"); // 好友数量
    if (num != 0){
        vector<string> friendlist = redis.Hgetall(msg.uid,"的好友列表");

        auto it = find(friendlist.begin(),friendlist.end(),msg.recuid);
        if (it != friendlist.end())
        {
            asocket.Send("exist"); 
            return;
        }
    }
    if (redis.hexists(msg.uid+"收到的好友申请",msg.recuid)){
        asocket.Send("apply");
        return;
    }
    if (redis.hexists(msg.uid +"收到的好友申请",msg.uid)){
        asocket.Send("handle");
        return;
    }
    string apply = "来自"+msg.uid+"的好友申请:"+msg.para[0];
    redis.Hset(msg.recuid+"收到的好友申请",msg.uid,apply);
    string nums = redis.Hget(msg.uid+"的未读消息","好友申请");
    redis.Hset(msg.recuid+"的未读消息","好友申请",to_string(stoi(nums)+1));
    // 给好友发送实时通知
    if (online_users.find(msg.recuid)!= online_users.end()){
        string friend_fd = redis.Hget(msg.recuid,"通知socket");
        TaskSocket friendsocket(stoi(friend_fd));
        string mesg = "收到来自"+msg.uid+"的好友申请";
        friendsocket.Send(RED+mesg+RESET);
    }
    asocket.Send("success");
}
void friend_del(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.uid+"的好友列表",msg.para[0])){
        asocket.Send("none"); // 不存在该好友
        return;
    }
    else if (redis.HValueremove(msg.uid+ "的好友列表",msg.para[0])){
        redis.HValueremove(msg.para[0]+"的好友列表",msg.uid);
        asocket.Send("success"); // 成功删除
        return;
    }
}
void friend_apply_agree(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.uid+"收到的好友申请",msg.para[0])){
        asocket.Send("notfind");
        return;
    }
    if (redis.HValueremove(msg.uid+"收到的好友申请",msg.para[0])){
        string nownum = redis.Hget(msg.uid+"的未读消息","好友申请");
        redis.Hset(msg.uid+"的未读消息","好友申请",(to_string(stoi(nownum) - 1)));

        redis.Hset(msg.uid+"的好友列表",msg.para[0],"good");
        redis.Rpushvalue(msg.uid+"与"+msg.para[0]+"的聊天记录","------");

        redis.Hset(msg.para[0]+"的好友列表",msg.uid,"good");
        redis.Rpushvalue(msg.para[0]+"与"+msg.uid+"的聊天记录","------");
        redis.Rpushvalue(msg.para[0]+ "的通知消息",msg.uid+"通过了你的好友申请");
        string num1 = redis.Hget(msg.para[0]+"的未读消息","通知类消息");
        redis.Hset(msg.para[0]+ "的未读消息","通知类消息",to_string(stoi(num1)+1));

        if (online_users.find(msg.para[0])!=online_users.end()){
            string friend_fd = redis.Hget(msg.para[0],"通知socket");
            TaskSocket friendsocket(stoi(friend_fd));
            friendsocket.Send(RED+msg.uid+"通过了你的好友申请"+RESET);
        }
        asocket.Send("success");
        return;
    }
}
void friend_apply_refuse(TaskSocket asocket,Message msg){
    if (!redis.hexists(msg.uid+"收到的好友申请",msg.para[0])){
        asocket.Send("notfind");
        return;
    }
    if (redis.HValueremove(msg.uid+"收到的好友申请",msg.para[0])){
        string nownum = redis.Hget(msg.uid+"的未读消息","好友申请");
        redis.Hset(msg.uid+"的未读消息","好友申请",(to_string(stoi(nownum) - 1)));

        string num1 = redis.Hget(msg.para[0]+"的未读消息","通知类消息");
        redis.Hset(msg.para[0]+"的未读消息","通知类消息",to_string(stoi(num1)+1));
        redis.Rpushvalue(msg.para[0]+"的通知消息",msg.uid+"拒绝了你");

        if (online_users.find(msg.para[0])!=online_users.end()){
            string friend_fd = redis.Hget(msg.para[0],"通知套接字");
            TaskSocket friendsocket(stoi(friend_fd));
            friendsocket.Send(RED+msg.uid+"拒绝了你"+RESET);
        }
        asocket.Send("success");
    }
}
void friend_block(TaskSocket asocket,Message msg)
{
    if (!redis.hexists(msg.uid+"的好友列表",msg.para[0])){
        asocket.Send("none"); 
        return;
    }
    if (redis.Sadd(msg.uid+"的屏蔽列表",msg.para[0])){
        asocket.Send("success"); // 成功
    }else{
        asocket.Send("handled"); // 已屏蔽过
    }
}
void friend_restore(TaskSocket asocket,Message msg)
{ 
    if (!redis.Sismember(msg.uid+"的屏蔽列表",msg.para[0])){
        asocket.Send("failure"); // 失败
        return;
    }
    if (!redis.hexists(msg.uid+"的好友列表",msg.para[0])){
        asocket.Send("none"); // 不存在
        return;
    }
    if (redis.Valueremove(msg.uid+"的屏蔽列表",msg.para[0]))
    asocket.Send("success"); // 成功
    else 
    asocket.Send("error"); // 失败
    return;
}
void friend_chat(TaskSocket asocket,Message msg){
    if(!redis.hexists(msg.uid+"的好友列表",msg.recuid)){
    asocket.Send("notfind");
    return;
    }
    // cout<<"什么鬼啊"<<endl;
    asocket.Send("success"); 
    vector<string> history = redis.Lrangeall(msg.uid+"与"+msg.recuid+"的聊天记录");
    cout<<"开始发送聊天记录"<<endl;
    for (const string &msg : history){
        asocket.Send(msg);
    }
    redis.Hset(msg.uid,"聊天对象",msg.recuid);

    asocket.Send("The end");
}
void friend_sendmsg(TaskSocket asocket,Message msg){
    string newmsg = "我:"+msg.para[0];
    redis.Rpushvalue(msg.uid+"与"+msg.recuid+"的聊天记录",newmsg);

    string my_recvfd = redis.Hget(msg.uid,"通知socket");
    TaskSocket my_socket(stoi(my_recvfd));
    my_socket.Send(newmsg);

    if (redis.Sismember(msg.recuid +"的屏蔽列表",msg.uid)){
        string msg = "对方拒绝了你的消息";
        my_socket.Send(RED+msg+RESET);
        asocket.Send("failure");
        return;
    }
    string uid = msg.uid;
    string msg1 = uid+":"+msg.para[0];
    redis.Rpushvalue(msg.recuid +"与"+msg.uid+"的聊天记录",msg1);

    if (online_users.find(msg.recuid)!=online_users.end() && (redis.Hget(msg.recuid ,"聊天对象") == msg.uid)){
        cout<<"在线"<<endl;
        string fr_recvfd = redis.Hget(msg.recuid ,"通知socket");
        TaskSocket fr_socket(stoi(fr_recvfd));
        fr_socket.Send(GREEN+msg1+RESET);
    }else if (online_users.find(msg.recuid)==online_users.end()){
    cout<<"不在线"<<endl;
        string num = redis.Hget(msg.recuid +"的未读消息","通知类消息");
        redis.Hset(msg.recuid +"的未读消息","通知类消息",to_string(stoi(num)+1));
        redis.Rpushvalue(msg.recuid +"的通知消息",msg.uid+"给你发来了一条消息");
    }else{
        cout<<"在线但是没在和你聊天"<<endl;
        string fr_recvfd = redis.Hget(msg.recuid ,"通知socket");
        TaskSocket fr_socket(stoi(fr_recvfd));
        fr_socket.Send(RED+msg.uid+"给你发来了一条消息"+RESET);
    }
    asocket.Send("success");
    return;
}
void Exitchat(TaskSocket mysocket,Message msg){
    redis.Hset(msg.uid,"聊天对象","0");
    redis.Hset(msg.recuid,"聊天对象","0");
    mysocket.Send("success");
    return;
}