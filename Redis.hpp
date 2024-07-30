#include<iostream>
#include<hiredis/hiredis.h>
#include<errno.h>
#include<stdexcept>
extern Redis redis;
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
    private :
    redisContext *con;
};