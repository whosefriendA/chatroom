#include"server.hpp"
void Sign_up(TaskSocket asocket,Message msg){
    string uid=to_string(User_uid);
    User u(msg.name,msg.pass,msg.question,msg.para[0],uid);
    cout<<u.UID<<u.Name<<u.Pass<<u.Question<<u.Answer<<endl;
    User_uid++;
    User_count++;
    //cout<<User_count<<endl;
    u.user_mem();
    redis.Sadd("用户uid集合",u.UID);
    redis.Hset(u.UID, "聊天对象", "无");
    redis.Hset(u.UID + "的未读消息", "通知类消息", "0");
    redis.Hset(u.UID + "的未读消息", "好友申请", "0");
    redis.Hset(u.UID + "的未读消息", "群聊消息", "0");
    asocket.Send(u.UID);
    return ;
}
void Log_in(TaskSocket asocket,Message msg){
    int a=redis.Sismember("用户uid集合",msg.uid);
    cout<<a<<endl;
    if(!a){
        asocket.Send("havenotsignup");
    }else{
    string pwd=redis.Sget(msg.uid,"pass");
    if(pwd!=msg.para[0]){
        asocket.Send("notcorrect");
    }else{
        redis.Sadd(online_users,msg.uid);

    redis.Hset("fd_uid表",to_string(asocket.getfd()),msg.uid);
    redis.Hset(msg.uid,"聊天对象","0");
    redis.Hset(msg.uid,"通知socket","-1");
    asocket.Send("success");
    cout<<"用户"<<msg.uid<<"登陆成功"<<endl;
    }
  }
};
void question_get(TaskSocket asocket,Message msg){
    string mesg=redis.Sget(msg.uid,"question");
    asocket.Send(mesg);
    return;
}
void pass_find(TaskSocket asocket,Message msg){
    string mesg=redis.Sget(msg.uid,"answer");
    asocket.Send(mesg);
    return;
}

void pass_get(TaskSocket asocket,Message msg){
    string mesg=redis.Sget(msg.uid,"pass");
    asocket.Send(mesg);
    return;
}

void User_remove(TaskSocket asocket, Message msg)//部分实现
{
    redis.Valueremove("用户uid集合", msg.uid); // 从用户uid集合中移除
    asocket.Send("success");
}
void Unreadnotice(TaskSocket asocket, Message msg){
    string notice;
    int nnum = stoi(redis.Hget(msg.uid + "的未读消息", "通知类消息"));
    int rnum = stoi(redis.Hget(msg.uid + "的未读消息", "好友申请"));
    int snum = nnum + rnum;
    if (snum == 0){
        asocket.Send("failure");
        return;
    }else{
        notice = "有" + to_string(snum) + "条未读消息:\n";
        for (int i = 0; i < nnum; ++i){
            string notify = msg.uid + "的通知消息";
            string notification = redis.LValueget(notify, i);
            notice += "通知" + to_string(i + 1) + ":" + notification + "\n";
        }
        vector<string> fieldNames = redis.Hgetall(msg.uid, "收到的好友申请");
        for (size_t i = 0; i < fieldNames.size(); ++i){
            string friendRequest = redis.Hget(msg.uid + "收到的好友申请", fieldNames[i]);
            notice += "好友申请" + to_string(i + 1) + ":" + friendRequest + "\n";
        }
    }
    asocket.Send(YELLOW + notice + RESET);
    redis.keyremove(msg.uid + "的通知消息");
    redis.Hset(msg.uid + "的未读消息", "通知类消息", "0");
}