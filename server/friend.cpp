#include"server.hpp"
// void friend_list_get(TaskSocket asocket, json command){
//     if (!redis.exists(command.at("UID") + "的好友列表")){
//         asocket.Sendmsg("none");
//         return;
//     }
//     vector<string> friendList = redis.getFriendList(command.at("UID"), "的好友列表");
//     for (const string &friendID : friendList){
//         if (!redis.Sismember(command.at("UID") + "的屏蔽列表", friendID)){
//             if (onlineUsers.find(friendID) != onlineUsers.end()){
//                 asocket.Sendmsg(L_GREEN + friendID + NONE);
//             }else{
//                 asocket.Sendmsg(friendID);
//             }
//         }
//     }
//     asocket.Sendmsg("end"); // 展示完毕
// }
void freind_add(TaskSocket asocket, json command){
     
}
void friend_del(TaskSocket asocket, json command){

}
void friend_apply(TaskSocket asocket, json command){

}