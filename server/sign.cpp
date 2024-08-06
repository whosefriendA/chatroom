#include"server.hpp"
void Sign_up(TaskSocket asocket,Message msg){
    string uid=to_string(User_uid);
    User u(msg.name,msg.pass,msg.question,msg.option[0],uid);
    cout<<u.UID<<u.Name<<u.Pass<<u.Question<<u.Answer<<endl;
    User_uid++;
    User_count++;
    //cout<<User_count<<endl;
    u.user_mem();
    redis.Sadd("用户uid集合",u.UID);
    redis.Hset(u.UID, "聊天对象", "无");
    redis.Hset(u.UID + "的未读消息", "通知消息", "0");
    redis.Hset(u.UID + "的未读消息", "好友申请", "0");
    redis.Hset(u.UID + "的未读消息", "群聊消息", "0");
    asocket.Sendmsg(u.UID);
    return ;
}
void Log_in(TaskSocket asocket,Message msg){
    int a=redis.Sismember("用户uid集合",msg.uid);
    cout<<a<<endl;
    if(!a){//帐号不存在
        asocket.Sendmsg("havenotsignup");
    }else{
    string pwd=redis.Sget(msg.uid,"pass");
    if(pwd!=msg.option[0]){
        asocket.Sendmsg("notcorrect");
    }else{
        redis.Sadd(online_users,msg.uid);
    //密码正确改变状态
    redis.Hset("fd_uid表",to_string(asocket.getfd()),msg.uid);
    redis.Hset(msg.uid,"聊天对象","0");
    redis.Hset(msg.uid,"通知socket","-1");
    asocket.Sendmsg("success");
    cout<<"用户"<<msg.uid<<"登陆成功"<<endl;
    }
  }
};
void question_get(TaskSocket asocket,Message msg){
    string mesg=redis.Sget(msg.uid,"question");
    asocket.Sendmsg(mesg);
    return;
}
void pass_find(TaskSocket asocket,Message msg){
    string mesg=redis.Sget(msg.uid,"answer");
    asocket.Sendmsg(mesg);
    return;
}

void pass_get(TaskSocket asocket,Message msg){
    string mesg=redis.Sget(msg.uid,"pass");
    asocket.Sendmsg(mesg);
    return;
}

void User_remove(TaskSocket asocket, Message msg)//部分实现
{
    redis.Valueremove("用户uid集合", msg.uid); // 从用户uid集合中移除
    asocket.Sendmsg("success");
}