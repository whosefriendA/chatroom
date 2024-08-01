#include"server.hpp"
void Sign_up(TaskSocket asocket,json command){

    string a=command.at("name");
    string b=command.at("pass");
    string c=command.at("question");
    string d=command.at("answer");
    string e=to_string(User_uid);
    User u(a,b,c,d,e);
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
void Log_in(TaskSocket asocket,json command){
    int a=redis.Sismember("用户uid集合",command.at("UID"));
    cout<<a<<endl;
    if(!a){//帐号不存在
        asocket.Sendmsg("havenotsignup");
    }else{
        //string pwd=redis.Hget("users",command.at("UID"));
    string pwd=redis.Sget(command.at("UID"),"pass");
    if(pwd!=command.at("option")[0]){
        asocket.Sendmsg("notcorrect");
    }else{
        redis.Sadd(online_users,command.at("UID"));
    //密码正确改变状态
    redis.Hset("fd_uid表",to_string(asocket.getfd()),command.at("UID"));
    redis.Hset(command.at("UID"),"聊天对象","0");
    redis.Hset(command.at("UID"),"通知socket","-1");
    asocket.Sendmsg("success");
    cout<<"用户"<<command.at("UID")<<"登陆成功"<<endl;
    }
  }
};
void question_get(TaskSocket asocket,json command){
    string msg=redis.Sget(command.at("UID"),"question");
    asocket.Sendmsg(msg);
    return;
}
void pass_find(TaskSocket asocket,json command){
    string msg=redis.Sget(command.at("UID"),"answer");
    asocket.Sendmsg(msg);
    return;
}

void pass_get(TaskSocket asocket,json command){
    string msg=redis.Sget(command.at("UID"),"pass");
    asocket.Sendmsg(msg);
    return;
}

void User_remove(TaskSocket asocket, json command)//部分实现
{
    redis.Valueremove("用户uid集合", command.at("UID")); // 从用户uid集合中移除
    asocket.Sendmsg("ok");
}