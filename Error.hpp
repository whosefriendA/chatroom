#ifndef _ERROR_
#define _ERROR_
#include<iostream>
using namespace std;
class Error{
    public:
    Error()=default;
    ~Error()=default;
    void server_close(int ret){
        if(ret==0||ret==-1){
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }
    }
    void server_close(string recv){
        if(recv=="close"){
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }
    }

};

#endif