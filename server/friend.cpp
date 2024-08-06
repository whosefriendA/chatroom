#include"server.hpp"
void friend_list_get(TaskSocket asocket, Message msg){
    if (!redis.exists(msg.uid + "的好友列表")){
        asocket.Sendmsg("none");
        return;
    }
    vector<string> friendList = redis.Hgetall(msg.uid, "的好友列表");
    for (const string &friendID : friendList){
        if (!redis.Sismember(msg.uid + "的屏蔽列表", friendID)){
            if (!redis.Sismember(online_users,friendID)){
                asocket.Sendmsg(YELLOW + friendID + RESET);
            }else{
                asocket.Sendmsg(friendID);
            }
        }
    }
    asocket.Sendmsg("end");
}
void friend_add(TaskSocket asocket, Message msg){
    if (!redis.Sismember("用户uid集合", msg.recuid)){
        asocket.Sendmsg("none"); // 该用户不存在
        return;
    }
    int num = redis.Sgetcount(msg.uid, "的好友列表"); // 好友数量
    if (num != 0){
        vector<string> friendlist = redis.Hgetall(msg.uid, "的好友列表");

        auto it = find(friendlist.begin(), friendlist.end(), msg.recuid);
        if (it != friendlist.end())
        {
            asocket.Sendmsg("exist"); 
            return;
        }
    }
    // 自己系统消息里有对方发来的未处理的好友申请，不能向对方发
    if (redis.hexists(msg.uid + "收到的好友申请", msg.recuid)){
        asocket.Sendmsg("apply");
        return;
    }
    // 已经向对方发申请未被处理，不能再次发送
    if (redis.hexists(msg.uid  + "收到的好友申请", msg.uid)){
        asocket.Sendmsg("handle");
        return;
    }
    string apply = "来自" + msg.uid + "的好友申请:" + msg.option[0];
    redis.Hset(msg.recuid + "收到的好友申请", msg.uid, apply);
    string nums = redis.Hget(msg.uid + "的未读消息", "好友申请");
    redis.Hset(msg.recuid + "的未读消息", "好友申请", to_string(stoi(nums) + 1));
    // 给好友发送实时通知
    if (redis.Sismember(online_users,msg.recuid)){
        string friend_fd = redis.Hget(msg.recuid, "通知套接字");
        TaskSocket friendsocket(stoi(friend_fd));
        string mesg = "收到来自" + msg.uid + "的好友申请";
        friendsocket.Sendmsg(RED + mesg + RESET);
    }
    asocket.Sendmsg("success");
}
void friend_del(TaskSocket asocket, Message msg){
    if (!redis.hexists(msg.uid + "的好友列表", msg.option[0])){
        asocket.Sendmsg("none"); // 不存在该好友
        return;
    }
    else if (redis.HValueremove(msg.uid+ "的好友列表", msg.option[0])){
        redis.HValueremove(msg.option[0] + "的好友列表", msg.uid);
        asocket.Sendmsg("success"); // 成功删除
        return;
    }
}
void friend_apply_agree(TaskSocket asocket, Message msg){
    {
    // 查看通知消息里有没有他的申请
    if (!redis.hexists(msg.uid + "收到的好友申请", msg.option[0])){
        asocket.Sendmsg("nofind");
        return;
    }
    if (redis.HValueremove(msg.uid + "收到的好友申请", msg.option[0])){
        string nownum = redis.Hget(msg.uid + "的未读消息", "好友申请");
        redis.Hset(msg.uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));
        // 同意者信息
        redis.Hset(msg.uid + "的好友列表", msg.option[0], "hello");
        redis.Rpushvalue(msg.uid + "和" + msg.option[0] + "的聊天记录", "-------------------");
        // 申请者信息
        redis.Hset(msg.option[0] + "的好友列表", msg.uid, "hello");
        redis.Rpushvalue(msg.option[0] + "和" + msg.uid + "的聊天记录", "-------------------");
        redis.Rpushvalue(msg.option[0]+ "的通知消息", msg.uid + "通过了您的好友申请");
        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.Hget(msg.option[0] + "的未读消息", "通知消息");
        redis.Hset(msg.option[0]+ "的未读消息", "通知消息", to_string(stoi(num1) + 1));
        // 给好友发送实时通知
        if (redis.Sismember(online_users,msg.option[0])){
            string friend_fd = redis.Hget(msg.option[0], "通知套接字");
            TaskSocket friendsocket(stoi(friend_fd));
            friendsocket.Sendmsg(RED + msg.uid + "通过了您的好友申请" + RESET);
        }
        asocket.Sendmsg("success");
        return;
    }
}
}
void friend_apply_refuse(TaskSocket asocket,Message msg){
        // 查看通知消息里有没有他的申请
    if (!redis.hexists(msg.uid + "收到的好友申请", msg.option[0]))
    {
        asocket.Sendmsg("nofind");
        return;
    }
    if (redis.HValueremove(msg.uid + "收到的好友申请", msg.option[0]))
    {
        string nownum = redis.Hget(msg.uid + "的未读消息", "好友申请");
        redis.Hset(msg.uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));

        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.Hget(msg.option[0] + "的未读消息", "通知消息");
        redis.Hset(msg.option[0] + "的未读消息", "通知消息", to_string(stoi(num1) + 1));
        redis.Rpushvalue(msg.option[0] + "的通知消息", msg.uid + "拒绝了您的好友申请");

        // 给好友发送实时通知
        if (redis.Sismember(online_users,msg.option[0]))
        {
            string friend_fd = redis.Hget(msg.option[0], "通知套接字");
            TaskSocket friendsocket(stoi(friend_fd));
            friendsocket.Sendmsg(RED + msg.uid + "拒绝了您的好友申请" + RESET);
        }
        asocket.Sendmsg("success");
    }
}
void friend_block(TaskSocket asocket, Message msg)
{
    if (!redis.hexists(msg.uid + "的好友列表", msg.option[0]))
    {
        asocket.Sendmsg("none"); // 不存在该好友
        return;
    }
    // 使用 Redis Set 的 SADD 命令将好友添加到屏蔽列表中
    bool success = redis.Sadd(msg.uid + "的屏蔽列表", msg.option[0]);

    if (success)
    {
        asocket.Sendmsg("success"); // 屏蔽成功
    }
    else
    {
        asocket.Sendmsg("handled"); // 已屏蔽过该好友
    }
}
void friend_restore(TaskSocket asocket, Message msg)
{
    // 使用 Redis Set 的 SISMEMBER 命令检查好友是否在用户的屏蔽列表中
    bool isBlocked = redis.Sismember(msg.uid + "的屏蔽列表", msg.option[0]);

    if (!isBlocked)
    {
        asocket.Sendmsg("no"); // 该好友未被屏蔽
        return;
    }

    // 使用 Redis Hash 的 HEXISTS 命令检查好友是否存在于用户的好友列表中
    bool isFriend = redis.hexists(msg.uid + "的好友列表", msg.option[0]);

    if (!isFriend)
    {
        asocket.Sendmsg("none"); // 不存在该好友
        return;
    }

    // 使用 Redis Set 的 SREM 命令将好友从屏蔽列表中移除
    bool success = redis.Valueremove(msg.uid + "的屏蔽列表", msg.option[0]);

    if (success)
    {
        // cout<<"1"<<endl;
        asocket.Sendmsg("success"); // 好友成功解除屏蔽
    }
    else
    {
        asocket.Sendmsg("error"); // 解除屏蔽失败
    }
    return;
}