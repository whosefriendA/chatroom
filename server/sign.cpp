#include"server.hpp"
void Sign_up(TaskSocket asocket,json command){
    string a=command.at("name");
    string b=command.at("pass");
    string c=command.at("question");
    string d=command.at("answer");
    User u(a,b,c,d);
    User_count++;
    asocket.Sendmsg(u.UID);
    u.user_mem();
    return ;
}
void Log_in(TaskSocket asocket,json command){
    redisReply* reply = (redisReply*)redisCommand(redis.con, "SISMEMBER %s %s", "uid集合", command.at("UID"));
    if(!reply){//帐号不存在返回错误
        asocket.Sendmsg("havenotsignup");
        freeReplyObject(reply);
    }else{
    freeReplyObject(reply);
    redisReply* reply=(redisReply*)redisCommand(redis.con,"SGET %s %s",command.at("UID"),"pass");
    string pwd=reply->str;
    freeReplyObject(reply);
    if(pwd!=command.at("option")[0]){
        asocket.Sendmsg("notcorrect");
    }else{
    void * reply=redisCommand(redis.con,"SADD %s %s",online_users.c_str(),command.at("UID"));
    freeReplyObject(reply);
    //密码正确改变状态
    void * reply=redisCommand(redis.con,"HSET %s %s %s","fd_uid表",to_string(asocket.getfd()),command.at("UID"));
    freeReplyObject(reply);
    void * reply=redisCommand(redis.con,"HSET %s %s %s",command.at("UID"),"聊天对象","0");
    freeReplyObject(reply);
    void * reply=redisCommand(redis.con,"HSET %s %s %s",command.at("UID"),"通知socket","-1");
    freeReplyObject(reply);
    asocket.Sendmsg("success");
    cout<<"用户"<<command.at("UID")<<"登陆成功"<<endl;
        }
    }
};
