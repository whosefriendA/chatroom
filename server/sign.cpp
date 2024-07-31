#include"server.hpp"
void Sign_up(TaskSocket asocket,json command){

    string a=command.at("name");
    string b=command.at("pass");
    string c=command.at("question");
    string d=command.at("answer");
    User u(a,b,c,d);
    cout<<u.UID<<u.Name<<u.Pass<<u.Question<<u.Answer<<endl;
    User_count++;
    cout<<User_count<<endl;
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
    redisReply* reply = (redisReply*)redisCommand(redis.con, "SISMEMBER %s %s", "uid集合", command.at("UID"));
    if(!reply){//帐号不存在返回错误
        asocket.Sendmsg("havenotsignup");
        freeReplyObject(reply);
    }else{
    freeReplyObject(reply);
    reply=(redisReply*)redisCommand(redis.con,"SGET %s %s",command.at("UID"),"pass");
    string pwd=reply->str;
    freeReplyObject(reply);
    if(pwd!=command.at("option")[0]){
        asocket.Sendmsg("notcorrect");
    }else{
    void * reply2=redisCommand(redis.con,"SADD %s %s",online_users.c_str(),command.at("UID"));
    freeReplyObject(reply2);
    //密码正确改变状态
    reply2=redisCommand(redis.con,"HSET %s %s %s","fd_uid表",to_string(asocket.getfd()),command.at("UID"));
    freeReplyObject(reply2);
    reply2=redisCommand(redis.con,"HSET %s %s %s",command.at("UID"),"聊天对象","0");
    freeReplyObject(reply2);
    reply2=redisCommand(redis.con,"HSET %s %s %s",command.at("UID"),"通知socket","-1");
    freeReplyObject(reply2);
    asocket.Sendmsg("success");
    cout<<"用户"<<command.at("UID")<<"登陆成功"<<endl;
    }
  }
};
void question_get(TaskSocket asocket,json command){
    redisReply *reply=(redisReply*)redisCommand(redis.con,"SGET %s %s",command.at("UID"),"question");
    string msg=reply->str;
    freeReplyObject(reply);
    asocket.Sendmsg(msg);
    return;
}
void pass_find(TaskSocket asocket,json command){
    redisReply *reply=(redisReply*)redisCommand(redis.con,"SGET %s %s",command.at("UID"),"answer");
    string msg=reply->str;
    freeReplyObject(reply);
    asocket.Sendmsg(msg);
    return;
}
void pass_get(TaskSocket asocket,json command){
    redisReply *reply=(redisReply*)redisCommand(redis.con,"SGET %s %s",command.at("UID"),"");
    string msg=reply->str;
    freeReplyObject(reply);
    asocket.Sendmsg(msg);
    return;
}