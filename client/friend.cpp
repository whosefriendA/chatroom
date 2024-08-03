#include"./client.hpp"
void friend_list_get(){
    string uid,friend_list;
    uid=get_uid();
    json data;
    data["UID"]=uid;
    data["flag"]=FRIEND_LIST;
    string command=data.dump();
    int ret=asocket.Sendmsg(command);
    err.server_close(ret);
    string recv=asocket.Recvmsg();
    err.server_close(recv);
}
void freind_add(){
    
}
void friend_del(){
    
}
void friend_apply(){
    
}