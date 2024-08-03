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
    if (!redis.hexists(command.at("UID") + "的好友列表", command.at("option"))){
        asocket.Sendmsg("none"); // 不存在该好友
        return;
    }
    else if (redis.HValueremove(command.at("UID") + "的好友列表", command.at("option"))){
        redis.HValueremove(command.at("option") + "的好友列表", command.at("UID"));
        asocket.Sendmsg("success"); // 成功删除
        return;
    }
}
void friend_apply_agree(TaskSocket asocket, json command){
    {
    string uid=command.at("UID");
    string option=command.at("option");
    // 查看通知消息里有没有他的申请
    if (!redis.hexists(uid + "收到的好友申请", option)){
        asocket.Sendmsg("nofind");
        return;
    }
    if (redis.HValueremove(uid + "收到的好友申请", option)){
        string nownum = redis.Hget(uid + "的未读消息", "好友申请");
        redis.Hset(uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));
        // 同意者信息
        redis.Hset(uid + "的好友列表", option, "hello");
        redis.Rpushvalue(uid + "和" + option + "的聊天记录", "-------------------");
        // 申请者信息
        redis.Hset(option + "的好友列表", uid, "hello");
        redis.Rpushvalue(option + "和" + uid + "的聊天记录", "-------------------");
        redis.Rpushvalue(option+ "的通知消息", uid + "通过了您的好友申请");
        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.Hget(option + "的未读消息", "通知消息");
        redis.Hset(option+ "的未读消息", "通知消息", to_string(stoi(num1) + 1));
        // 给好友发送实时通知
        if (redis.Sismember(online_users,option)){
            string friend_fd = redis.Hget(option, "通知套接字");
            TaskSocket friendsocket(stoi(friend_fd));
            friendsocket.Sendmsg(RED + uid + "通过了您的好友申请" + RESET);
        }
        asocket.Sendmsg("success");
        return;
    }
}
}
void friend_apply_refuse(TaskSocket asocket,json command){
    
}