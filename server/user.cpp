#include "server.hpp"
User::User(string name,string pass,string question,string answer){
    this->UID=FID+User_count;
    this->Name=name;
    this->Pass=pass;
    this->Question=question;
    this->Answer=answer;
}
void User::user_mem(){
    json people;
    people["name"]=Name;
    people["pass"]=Pass;
    people["question"]=Question;
    people["answer"]=Answer;
    people["friend_list"]=friend_list;
    people["group_list"]=group_list;
    string info=people.dump();
    redisReply* reply=(redisReply*)redisCommand(redis.con,"SADD %s %s","UID",info);
    freeReplyObject(reply);
}
//好友相关
void User::friend_add(string friend_id){}
void User::friend_delete(string friend_id){}
void User::friend_shield(string friend_id){}
void User::friend_recover(string friend_id){}
//群相关
void User::group_add(string group_id){}
void User::group_delete(string group_id){}
void User::group_shield(string group_id){}
void User::group_recover(string group_id){}