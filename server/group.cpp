#include"server.hpp"
void group_create(TaskSocket asocket,Message msg){
       // 判断好友列表里有没有该好友
    if (!redis.hexists(msg.uid + "的好友列表", msg.para[0])){
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

    redis.Sadd("群聊集合", groupuid);
    redis.Hset(groupuid + "的详情", "群号", groupuid);
    redis.Hset(groupuid + "的详情", "群名称", groupuid);
    redis.Hset(groupuid + "的详情", "群介绍", "--");
    redis.Hset(groupuid + "群成员列表",msg.uid, "群主");
    redis.Hset(msg.uid + "的群聊列表",groupuid, "群主");
    redis.Rpushvalue(groupuid + "的群聊消息", "------");

    string num = redis.Hget(msg.para[0] + "的未读消息", "通知类消息");
    redis.Hset(msg.para[0] + "的未读消息", "通知类消息", to_string(stoi(num) + 1));
    redis.Rpushvalue(msg.para[0] + "的通知消息", msg.uid + "邀请你加入群聊" + groupuid);

    redis.Hset(groupuid + "群成员列表", msg.para[0], "群成员");
    redis.Hset(msg.para[0] + "的群聊列表", groupuid, "群成员");
    if (online_users.find(msg.para[0]) != online_users.end()){
        string friend_fd = redis.Hget(msg.para[0], "通知socket");
        TaskSocket friendsocket(stoi(friend_fd));
        friendsocket.Send(BLUE + msg.uid + "邀请你加入群聊"+RESET);
    }
    asocket.Send(groupuid);
    return;
}
void group_add(TaskSocket asocket,Message msg){
    if (redis.hexists(msg.para[0] + "群成员列表", msg.uid)){
        asocket.Send("already");
        return;
    }else if (!redis.Sismember("群聊集合", msg.para[0])){
        asocket.Send("none");
        return;
    }else if (redis.hexists(msg.para[0] + "的申请加群列表", msg.uid)){
        asocket.Send("again");
        return;
    }else{
        string mesg = "申请加入群聊";
        redis.Hset(msg.para[0] + "的申请加群列表", msg.uid, mesg);
        vector<string> memberlist = redis.Hgetall(msg.para[0], "群成员列表");
        for (const string &memberid : memberlist){
            if (redis.Hget(msg.para[0] + "群成员列表", memberid) != "群成员"){
                string apply = msg.uid + "申请加入群聊" + msg.para[0];
                string num = redis.Hget(memberid + "的未读消息", "群聊消息");
                redis.Hset(memberid + "的未读消息", "群聊消息", to_string(stoi(num) + 1));
                redis.Rpushvalue(memberid + "群聊消息", apply);
                if(redis.Sismember("在线用户列表", memberid)){
                    string member_fd = redis.Hget(memberid, "通知socket");
                    TaskSocket membersocket(stoi(member_fd));
                    membersocket.Send(BLUE + apply + RESET);
                }
            }
        }
        asocket.Send("success");
        return;
    }
}
void group_in(TaskSocket asocket,Message msg){
    if (!redis.Sismember("群聊集合", msg.para[0])){
        asocket.Send("none");
        return;
    }else if (!redis.hexists(msg.para[0] + "群成员列表", msg.uid)){
        asocket.Send("failure");
        return;
    }else if (redis.hexists(msg.para[0] + "群成员列表", msg.uid)){
        asocket.Send("success");
        return;
    }
}
void group_list_get(TaskSocket asocket,Message msg){
    
}