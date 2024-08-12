#ifndef _REDIS_
#define _REDIS_
#include<iostream>
#include<hiredis/hiredis.h>
#include<vector>
#include<string>
using namespace std;
class Redis{
    public:
    Redis (){
        con=redisConnect("127.0.0.1",6379);
        if(con->err&&con==NULL){
            throw runtime_error("Wrong connect to redis");
        }
    }
    ~Redis(){
        redisFree(con);  
    }
    int Sadd(const string& key,const string& value) {
        redisReply* reply = (redisReply*)redisCommand(this->con,"SADD %s %s",key.c_str(),value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }
    int Scard(const string&key){
        int num;
        redisReply *reply = (redisReply *)redisCommand(this->con,"SCARD %s",key.c_str());
        if(reply!=nullptr&&reply->type==REDIS_REPLY_INTEGER){
            num=reply->integer;
            freeReplyObject(reply);
            cout<<"num="<<num<<endl;
            return num;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        num=0;
        cout<<"num="<<num<<endl;
        return num;
    }
    string Sget(const string&key,const string&filed){
        string value;
        redisReply*reply=(redisReply*)redisCommand(this->con,"SGET %s %s",key.c_str(),filed.c_str());
        if(reply!=nullptr&&reply->type==REDIS_REPLY_INTEGER&&reply->integer==-1){
            value=reply->str;
            freeReplyObject(reply);
            return value;
        }
        if(reply){
            freeReplyObject(reply);
        }
        return "";
    }
    vector<string> Sgetall(const string& key) {
    vector<string> members;
    redisReply* reply = (redisReply*)redisCommand(this->con,"SMEMBERS %s",key.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            members.push_back(reply->element[i]->str);
        }
        freeReplyObject(reply);
    } else {
        if (reply) {
            freeReplyObject(reply);
        }
    }
    return members;
    }
    vector<std::string> Hgetall(const string& key,const string& type) {
    string listkey=key+type;
    vector<string> friendlist;
    redisReply* reply = (redisReply*)redisCommand(this->con,"HKEYS %s",listkey.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            friendlist.push_back(reply->element[i]->str);
        }
        freeReplyObject(reply);
    }
    return friendlist;
    }
    int Hset(const string& key,const string& field,const string& value) {
        redisReply* reply = (redisReply*)redisCommand(this->con,"HSET %s %s %s",key.c_str(),field.c_str(),value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

    string Hget(const string& key,const string& field) {
        string value;
        redisReply* reply = (redisReply*)redisCommand(this->con,"HGET %s %s",key.c_str(),field.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
            value = reply->str;
            // cout<<"value="<<value<<endl;
            freeReplyObject(reply);
            return value;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        cout<<"not find"<<endl;
        return "";
    }
    bool hexists(const string& key,const string& field){
        // cout<<"key="<<key<<endl<<"filed="<<field<<endl;
        redisReply* reply = (redisReply*)redisCommand(this->con,"HEXISTS %s %s",key.c_str(),field.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool a=reply->integer;
            // cout<<"reply是"<<a<<endl;
            freeReplyObject(reply);
            return a;
        }
        // cout<<"错误"<<endl;
        freeReplyObject(reply);
        return false;
    }
    int Sismember(const string& key,const string& member) {
        redisReply* reply = (redisReply*)redisCommand(this->con,"SISMEMBER %s %s",key.c_str(),member.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool exists = (reply->integer == 1);
            freeReplyObject(reply);
            return exists;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }
    int Valueremove(const string& key,const string& member) {
    redisReply* reply = (redisReply*)redisCommand(this->con,"SREM %s %s",key.c_str(),member.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        bool success = (reply->integer > 0); 
        freeReplyObject(reply);
        return success;
    }
    return false; 
    }
    int keyremove(const string& key) {
        redisReply* reply = (redisReply*)redisCommand(this->con,"DEL %s",key.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool success = (reply->integer > 0); 
            return success;
        }
        return false;
    }
    int exists(const string& key) {
    redisReply* reply = (redisReply*)redisCommand(this->con,"EXISTS %s",key.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        bool exists = (reply->integer == 1);
        freeReplyObject(reply);
        return exists;
    }
    
    if (reply) {
        freeReplyObject(reply);
    }
    return false;
}
    int Sgetcount(const string& userID,const string& listType) {
        string listKey = userID+listType;

        redisReply* reply = (redisReply*)redisCommand(this->con,"HLEN %s",listKey.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            int listCount = static_cast<int>(reply->integer);
            freeReplyObject(reply);
            return listCount;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return 0;
    }
    bool HValueremove(const string& key,const string& field) {
    redisReply* reply = (redisReply*)redisCommand(this->con,"HDEL %s %s",key.c_str(),field.c_str());
    if (reply != nullptr&&reply->type==REDIS_REPLY_INTEGER) {
        bool success = (reply->integer > 0);
        return success;
    }
    return false;
    }
    bool Rpushvalue(const string& key,const string& value) {
    redisReply* reply = (redisReply*)redisCommand(this->con,"RPUSH %s %s",key.c_str(),value.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        bool success = (reply->integer >= 0);
        freeReplyObject(reply);
        return success;
    }
    return false;
}
string LValueget(const string& key,int index) {
    redisReply* reply = (redisReply*)redisCommand(this->con,"LINDEX %s %d",key.c_str(),index);
    if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
        string value = reply->str;
        freeReplyObject(reply);
        return value;
    }
    return "";
}
vector<string> Lrangeall(const std::string& key) {
        vector<string> result;
        // cout<<"lrange启动！"<<endl;
        redisReply* reply = (redisReply*)redisCommand(this->con,"LRANGE %s 0 -1",key.c_str());
        if (reply!=nullptr&&reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; ++i) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(reply->element[i]->str);
                }
            }
        }
        freeReplyObject(reply);
        return result;
    }
    private:
    redisContext *con;
};

#endif