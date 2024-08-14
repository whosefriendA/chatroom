#include"server.hpp"
void F_sendfile_toserver(TaskSocket asocket,Message msg){
    Sendfile_toserver(asocket,msg);
    string newmsg = "我:发送了一个文件"+msg.para[0];//入队列
    redis.Rpushvalue(msg.uid+"与"+msg.recuid+"的聊天记录",newmsg);
    //在发送者消息
    string my_recvfd = redis.Hget(msg.uid,"实时socket");
    TaskSocket my_socket(stoi(my_recvfd));
    my_socket.Send(newmsg);
    //被屏蔽
    if (redis.Sismember(msg.recuid+"的屏蔽列表",msg.uid)){
        string msg = "对方拒绝了你的消息";
        my_socket.Send(RED+msg+RESET);
        asocket.Send("failure");
        return;
    }
    string uid = msg.uid;
    string msg1 = uid+":发送了一个文件"+msg.para[0];
    redis.Rpushvalue(msg.recuid+"与"+msg.uid+"的聊天记录",msg1);
    // ta在线并在和我聊天
    if (online_users.find(msg.recuid) != online_users.end() && (redis.Hget(msg.recuid,"聊天对象") == msg.uid)){
        string fr_recvfd = redis.Hget(msg.recuid,"实时socket");
        TaskSocket fr_socket(stoi(fr_recvfd));
        fr_socket.Send(GREEN+msg1+RESET);
    }else if (online_users.find(msg.recuid) == online_users.end()) {//不在线
        string num = redis.Hget(msg.recuid+"的未读消息","通知类消息");
        redis.Hset(msg.recuid+"的未读消息","通知类消息",to_string(stoi(num)+1));
        redis.Rpushvalue(msg.recuid+"的通知消息",msg.uid+"发来了一个文件");
    }else{
        string fr_recvfd = redis.Hget(msg.recuid,"实时socket");
        TaskSocket fr_socket(stoi(fr_recvfd));
        fr_socket.Send(RED+msg.uid+"发来了一个文件"+RESET);
    }
    asocket.Send("success");
    return;
}
void F_receivefile_fromserver(TaskSocket asocket,Message msg){
    Receivefile_fromserver(asocket,msg);
    string newmsg = "我:成功接收了一个文件"+msg.para[0];
    redis.Rpushvalue(msg.uid+"与"+msg.recuid+"的聊天记录",newmsg);
    // 发送者消息
    string my_recvfd = redis.Hget(msg.uid,"实时socket");
    TaskSocket my_socket(stoi(my_recvfd));
    my_socket.Send(newmsg);
    // 被屏蔽
    if (redis.Sismember(msg.recuid+"的屏蔽列表",msg.uid)){
        string msg = "对方拒收了你的消息";
        my_socket.Send(RED+msg+RESET);
        asocket.Send("failure");
        return;
    }
    string uid = msg.uid;
    string msg1 = uid+":成功接收到一个文件"+msg.para[0];
    redis.Rpushvalue(msg.recuid+"与"+msg.uid+"的聊天记录",msg1);
    // 好友在线并与我聊天
    if ((online_users.find(msg.recuid) != online_users.end()) && (redis.Hget(msg.recuid,"聊天对象") == msg.uid)){
        string fr_recvfd = redis.Hget(msg.recuid,"实时socket");
        TaskSocket fr_socket(stoi(fr_recvfd));
        fr_socket.Send(GREEN+msg1+RESET);
    }else if (online_users.find(msg.recuid) == online_users.end()){// 好友不在线
        string num = redis.Hget(msg.recuid+"的未读消息","通知类消息");
        redis.Hset(msg.recuid+"的未读消息","通知类消息",to_string(stoi(num)+1));
        redis.Rpushvalue(msg.recuid+"的通知消息",msg.uid+"成功接收到你发送的文件");
    }else{
        string fr_recvfd = redis.Hget(msg.recuid,"实时socket");
        TaskSocket fr_socket(stoi(fr_recvfd));
        fr_socket.Send(RED+msg.uid+"成功接收到你发送的文件"+RESET);
    }
    return;
}
void G_sendfile_toserver(TaskSocket asocket,Message msg){
    Sendfile_toserver(asocket,msg);
    string newmsg = msg.uid+":发送了一个文件"+msg.para[0];
    redis.Rpushvalue(msg.recuid+"的群聊消息", newmsg);
    // 发送者消息
    string my_recvfd = redis.Hget(msg.uid, "实时socket");
    TaskSocket my_socket(stoi(my_recvfd));
    my_socket.Send(WIDEWHITE+newmsg+RESET);
    string uid = msg.uid;
    string msg1 = uid+":发送了一个文件"+msg.para[0];
    vector<string> memberlist = redis.Hgetall(msg.recuid, "群成员列表");
    for (const string &memberid : memberlist){
        if ((online_users.find(memberid) == online_users.end()) && memberid != uid){
            string apply = msg.recuid+"群聊中有人发来了一个新文件";
            string num = redis.Hget(memberid+"的未读消息", "群聊消息");
            redis.Hset(memberid+"的未读消息", "群聊消息", to_string(stoi(num)+1));
            redis.Rpushvalue(memberid+"群聊消息", apply);
        }else if ((online_users.find(msg.recuid) != online_users.end()) && (redis.Hget(memberid, "聊天对象") == msg.recuid) && memberid != uid){
            string gr_recvfd = redis.Hget(memberid, "实时socket");
            TaskSocket gr_socket(stoi(gr_recvfd));
            gr_socket.Send(GREEN+msg1+RESET);
        }else if ((online_users.find(msg.recuid) != online_users.end()) && memberid != uid){
            string apply = msg.recuid+"群聊中有人发来了一个新文件";
            string gr_recvfd = redis.Hget(memberid, "实时socket");
            TaskSocket gr_socket(stoi(gr_recvfd));
            gr_socket.Send(GREEN+apply+RESET);
        }
    }
    asocket.Send("success");
    return;
}
void G_receivefile_fromserver(TaskSocket asocket,Message msg){
    Receivefile_fromserver(asocket,msg);
}
void Sendfile_toserver(TaskSocket asocket,Message msg){
    string filename = msg.para[0];
    size_t filesize = stoul(msg.para[1]); 
    string savepath = "/home/wanggang/chatroom/file/"+msg.recuid+"/"; 
    string filepath = savepath+filename;
    // 创建目录
    int mkdirStatus = mkdir(savepath.c_str(),S_IRWXU | S_IRWXG | S_IRWXO);
    if (mkdirStatus != 0 && errno != EEXIST){
        cerr << "Error creating directory: " << strerror(errno) << endl;
    }
    asocket.Send("success");

    int filefd = open(filepath.c_str(),O_WRONLY | O_CREAT | O_APPEND,S_IRWXU);
    if (filefd == -1){
        cerr << "Error opening file for writing" << endl;
        asocket.Send("close");
        return;
    }
    // cout<<mysocket.getfd()<<endl;
    off_t offset = 0;
    ssize_t totalRecvByte = 0;
    char buf[BUFSIZ];
    lseek(filefd,0,SEEK_SET);

    while (filesize > totalRecvByte){
        ssize_t byteRead = read(asocket.getfd(),buf,BUFSIZ);
        cout << byteRead << endl;
        if (byteRead == -1){
            if (errno == EAGAIN || errno == EWOULDBLOCK){
                continue;
            }else{
                cerr << "Error reading file: " << strerror(errno) << endl;
            }
        }
        if (byteRead == 0){
            cerr << "Connection closed by client" << endl;
            break;
        }
        ssize_t byteWritten = write(filefd,buf,byteRead);
        if (byteWritten == -1){
            cerr << "Error writing to file" << endl;
            break;
        }
        totalRecvByte += byteWritten;
        cout << totalRecvByte << endl;
    }
    close(filefd);
}
void Receivefile_fromserver(TaskSocket asocket,Message msg){
    string filename = msg.para[0];
    string savepath = "/home/wanggang/chatroom/file/"+msg.uid+"/";
    string filepath = savepath+filename;
    int filefd = open(filepath.c_str(),O_RDONLY);
    if (filefd == -1){
        cerr << "Error opening file for writing" << endl;
        asocket.Send("close");
        return;
    }else{
        struct stat statbuf;
        fstat(filefd,&statbuf);
        int ret = asocket.Send(to_string(statbuf.st_size));
        if (ret == 0 || ret == -1){
            cout << "已关闭" << endl;
            exit(0);
        }
        ssize_t bytes_sent = 0;
        cout << statbuf.st_size << endl;
        while (bytes_sent < statbuf.st_size){
            ssize_t ret_send = sendfile(asocket.getfd(),filefd,&bytes_sent,statbuf.st_size - bytes_sent);
            cout << "statbuf:" << statbuf.st_size << endl;
            cout << "bytes:" << ret_send << endl;
            if (ret_send == -1){
                if (errno == EAGAIN || errno == EWOULDBLOCK){
                    // 继续尝试发送
                    continue;
                }else{
                    cerr << "Error sending file data: " << strerror(errno) << endl;
                    close(filefd);
                    break;
                }
            }else if (ret_send == 0){
                cerr << "Connection closed by peer while sending file data." << endl;
                break;
            }
            cout << bytes_sent << endl;
        }
    }
    close(filefd);
}
