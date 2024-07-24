#include<iostream>
#include<hiredis/hiredis.h>
#include<mutex>
int main(){
    redisContext *c=redisConnect("127.0.0.1",6379);
    if(c->err){
         printf("conect error");
         redisFree(c);
         return 0;
    }
    redisReply *reply=(redisReply *)redisCommand(c,"set foo 1234");
    freeReplyObject(reply);
    reply=(redisReply*)redisCommand(c,"get foo");
    std::cout<<reply->str;
    freeReplyObject(reply);
    redisFree(c);
}
