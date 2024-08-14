#include "server.hpp"
User::User(string name,string pass,string question,string answer,string UID){
    this->UID=UID;
    this->Name=name;
    this->Pass=pass;
    this->Question=question;
    this->Answer=answer;
}
void User::user_mem(){
    redis.Hset(UID,"name",Name);
    redis.Hset(UID,"pass",Pass);
    cout<<Pass<<endl;
    redis.Hset(UID,"question",Question);
    redis.Hset(UID,"answer",Answer);
}
//好友相关
// void User::friend_add(string friend_id){
// }
// void User::friend_delete(string friend_id){
// }

//群相关
void User::group_add(string group_id){}
void User::group_delete(string group_id){}
