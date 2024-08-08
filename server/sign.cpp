#include"server.hpp"

void Sign_up(TaskSocket asocket,Message msg){
    User_uid+=redis.Scard("uid集合");
    string uid=to_string(User_uid);
    User u(msg.name,msg.pass,msg.question,msg.para[0],uid);
    // cout<<u.UID<<u.Name<<u.Pass<<u.Question<<u.Answer<<endl;
    // cout<<User_uid<<endl;
    redis.Hset(uid,"name",msg.name);
    // cout<<"msg.name="<<msg.name<<endl;
    // cout<<"name="<<redis.Hget(uid,"name")<<endl;
    redis.Hset(uid,"pass",msg.pass);
    // cout<<"msg.pass="<<msg.pass<<endl;
    string pass=redis.Hget(uid,"pass");
    // cout<<"pass="<<pass<<endl;
    redis.Hset(uid,"question",msg.question);
    redis.Hset(uid,"answer",msg.para[0]);
    redis.Sadd("uid集合",uid);
    redis.Hset(uid, "聊天对象", "无");
    redis.Hset(uid + "的未读消息", "通知类消息", "0");
    redis.Hset(uid + "的未读消息", "好友申请", "0");
    redis.Hset(uid + "的未读消息", "群聊消息", "0");
    asocket.Send(uid);
    return;
}
void Log_in(TaskSocket asocket,Message msg){
    // cout<<msg.uid<<endl;
    int a=redis.Sismember("uid集合",msg.uid);
    // cout<<"a="<< a<<endl;
    if(!a){
        asocket.Send("havenotsignup");
    }else{
    string pwd=redis.Hget(msg.uid,"pass");
    cout<<pwd<<endl;
    if(pwd!=msg.para[0]){
        cout<<pwd<<endl;
        asocket.Send("notcorrect");
    }else{
        redis.Sadd(online_users,msg.uid);
        redis.Hset("fd_uid表",to_string(asocket.getfd()),msg.uid);
        redis.Hset(msg.uid,"聊天对象","0");
        redis.Hset(msg.uid,"通知socket","-1");
        string name=redis.Hget(msg.uid,"name");
        asocket.Send(name);
        cout<<"用户"<<msg.uid<<"登录成功"<<endl;
    }
  }
};
void question_get(TaskSocket asocket,Message msg){
    string mesg=redis.Hget(msg.uid,"question");
    asocket.Send(mesg);
    return;
}
void pass_find(TaskSocket asocket,Message msg){
    string mesg=redis.Hget(msg.uid,"answer");
    asocket.Send(mesg);
    return;
}

void pass_get(TaskSocket asocket,Message msg){
    string mesg=redis.Hget(msg.uid,"pass");
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
void getname(TaskSocket asocket, Message msg){

}