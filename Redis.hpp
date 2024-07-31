#include<iostream>
#include<hiredis/hiredis.h>
#include<errno.h>
#include<stdexcept>
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
    int Sadd(const string& key, const string& value) {
        redisReply* reply = (redisReply*)redisCommand(this->con, "SADD %s %s", key.c_str(), value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
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
    vector<string> Sgetall(const std::string& key) {
    vector<string> members;
    redisReply* reply = (redisReply*)redisCommand(this->con, "SMEMBERS %s", key.c_str());
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
    int Hset(const string& key, const string& field, const string& value) {
        redisReply* reply = (redisReply*)redisCommand(this->con, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

    string Hget(const string& key, const string& field) {
        string value;
        redisReply* reply = (redisReply*)redisCommand(this->con, "HGET %s %s", key.c_str(), field.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
            value = reply->str;
            freeReplyObject(reply);
            return value;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return "";
    }

    int Sismember(const string& key, const string& member) {
        redisReply* reply = (redisReply*)redisCommand(this->con, "SISMEMBER %s %s", key.c_str(), member.c_str());
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
    int Valueremove(const string& key, const string& member) {
    redisReply* reply = (redisReply*)redisCommand(this->con, "SREM %s %s", key.c_str(), member.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        bool success = (reply->integer > 0); 
        freeReplyObject(reply);
        return success;
    }
    return false; 
    }
    bool keyremove(const string& key) {
        redisReply* reply = (redisReply*)redisCommand(this->con, "DEL %s", key.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool success = (reply->integer > 0); 
            return success;
        }
        return false;
    }
    private:
    redisContext *con;
};