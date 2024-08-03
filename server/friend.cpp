#include"server.hpp"
void friend_list_get(TaskSocket asocket, json command){
    if (!redis.exists(command.at("UID") + "的好友列表")){
        asocket.Sendmsg("none");
        return;
    }
    vector<string> friendList = redis.Sgetall(command.at("UID"), "的好友列表");
    for (const string &friendID : friendList){
        if (!redis.Sismember(command.at("UID") + "的屏蔽列表", friendID)){
            if (!redis.Sismember(online_users,friendID)){
                asocket.Sendmsg(YELLOW + friendID + RESET);
            }else{
                asocket.Sendmsg(friendID);
            }
        }
    }
    asocket.Sendmsg("end");
}
void friend_add(TaskSocket asocket, json command){
    string uid=command.at("UID");
    string option=command.at("option");
    string recvuid=command.at("recvUID");
    if (!redis.Sismember("用户uid集合", recvuid)){
        asocket.Sendmsg("none"); // 该用户不存在
        return;
    }
    int num = redis.Sgetcount(uid, "的好友列表"); // 好友数量
    if (num != 0){
        vector<string> friendlist = redis.Sgetall(uid, "的好友列表");
        auto it = find(friendlist.begin(), friendlist.end(), recvuid);
        if (it != friendlist.end()){
            asocket.Sendmsg("exist");
            return;
        }
    }
    // 自己系统消息里有对方发来的未处理的好友申请，不能向对方发
    if (redis.hexists(uid + "收到的好友申请", recvuid)){
        asocket.Sendmsg("apply");
        return;
    }
    // 已经向对方发申请未被处理，不能再次发送
    if (redis.hexists(uid  + "收到的好友申请", uid)){
        asocket.Sendmsg("handle");
        return;
    }
    string apply = "来自" + uid + "的好友申请:" + option;
    redis.Hset(recvuid + "收到的好友申请", uid, apply);
    string nums = redis.Hget(uid + "的未读消息", "好友申请");
    redis.Hset(recvuid + "的未读消息", "好友申请", to_string(stoi(nums) + 1));
    // 给好友发送实时通知
    if (redis.Sismember(online_users,recvuid)){
        string friend_fd = redis.Hget(recvuid, "通知套接字");
        TaskSocket friendsocket(stoi(friend_fd));
        string msg = "您收到来自" + uid + "的好友申请";
        friendsocket.Sendmsg(RED + msg + RESET);
    }
    asocket.Sendmsg("success");
}
void friend_del(TaskSocket asocket, json command){
    
}
void friend_apply_agree(TaskSocket asocket, json command){

}
void friend_apply_refuse(TaskSocket asocket,json command){

}