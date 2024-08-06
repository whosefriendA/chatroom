#include"./client.hpp"
void friend_list_get(){
    Message msg(curuid,FRIEND_LIST); // 展示好友列表
    int ret = asocket.Send(msg.S_to_json());
    if (ret == 0||ret == -1){
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    string Friend;
    while ((Friend = asocket.Receive()) != "end"){
        if (Friend == "none"){
            cout << "你当前还没有好友" << endl;
            return;
        }else if (Friend == "close"){
            cout << "服务器端已关闭" << endl;
            exit(0);
        }else{
            cout << Friend << endl;
        }
    }
    cout << "黄色表示好友在线" << endl;
    return;
}
void freind_add(){
    string recv_uid;//被添加好友的uid
    string option;//发送的验证消息
    cout<<"您想添加的好友的uid为:"<<endl; 
    cin>>recv_uid;
    cout<<"您想发送的验证消息为"<<endl;
    cin.ignore();
    getline(cin,option);

    Message command1(curuid,recv_uid,ADD_FRIEND,{option});
    int ret = asocket.Send(command1.S_to_json());
    err.server_close(ret);
    string recv = asocket.Receive();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "好友添加申请已发送,等待对方通过" << endl;
        return ;
    }else if (recv == "exist"){
        system("clear");
        cout << "该用户已是好友,不能重复添加" << endl;
        return ;
    }else if(recv=="handle"){
        system("clear");
        cout<<"已向该好友发送过好友申请,请等待回复"<<endl;
        return ;
    }else if (recv == "apply"){
        system("clear");
        cout << "系统消息中存在对方发送的好友申请,请先回复" << endl;
        return ;
    }else if(recv=="none"){
        cout << "该用户不存在" << endl;
        return ;
    }else{
        cout<<"其他错误"<<endl;
        return ;
    }
}
void friend_del(){
    friend_list_get();
    string deluid;
    cout<<"请输入想删除的好友的uid:"<<endl;
    cin>>deluid;

    Message msg(curuid,DELETE_FRIEND,{deluid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv = asocket.Receive();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "已成功删除该好友" << endl;
        return ;
    }else if (recv == "none"){
        cout << "未找到该好友" << endl;
        return ;
    }else{
        cout<<"其他错误"<<endl;
        return ;
    }
}
void friend_apply_agree(){
    string agreeuid;//同意的uid
    cout<<"想同意加好友申请的好友uid为:"<<endl;
    cin>>agreeuid;

    Message msg(curuid,AGREE_ADDFRIEND,{agreeuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv = asocket.Receive();
    err.server_close(recv);
    if (recv == "success"){
        system("clear");
        cout << "已通过" << agreeuid << "的好友申请" << endl;
        return;
    }else if(recv=="notfind"){
        cout<<"不存在该用户的好友申请"<<endl;
        return ;
    }else{
        cout << "错误" << endl;
        return;
    }
}
void friend_apply_refuse(){
    friend_list_get();
    string refuseuid;//拒绝的uid
    cout<<"想拒绝加好友申请的uid为:"<<endl;
    cin>>refuseuid;

    Message msg(curuid,REFUSE_ADDFRIEND,{refuseuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="notfind"){
        cout<<"不存在该用户的好友申请"<<endl;
        return;
    }else if(recv=="ok"){
        system("clear");
        cout<<"已拒绝"<<refuseuid<<"的申请"<<endl;
        return;
    }else{
        cout<<recv<<endl;
        cout<<"错误"<<endl;
        return;
    }
}
void friend_block(){
    friend_list_get();
    string blockuid;//屏蔽好友
    cout<<"想屏蔽该好友uid为:"<<endl;
    cin>>blockuid;

    Message msg(curuid,BLOCK_FRIEND,{blockuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="success"){
        cout<<"已屏蔽该好友"<<endl;
        return;
    }else if(recv=="none"){
        cout<<"未找到该好友"<<endl;
        return;
    }else if(recv=="handled"){
        system("clear");
        cout<<"已屏蔽过该好友,重复操作"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void friend_restore(){
    string restoreuid;//恢复好友会话
    cout<<"想恢复聊天的好友uid为:"<<endl;
    cin>>restoreuid;

    Message msg(curuid,RESTORE_FRIEND,{restoreuid});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="success"){
        cout<<"已恢复与它聊天"<<endl;
        return;
    }else if(recv=="none"){
        cout<<"未找到该好友"<<endl;
        return;
    }else if(recv=="no"){
        system("clear");
        cout<<"该好友未被屏蔽"<<endl;
        return;
    }else{
        cout<<"错误"<<endl;
        return;
    }
}
void friend_chat()
{
    friend_list_get();
    string recvuid;
    cout<<"想聊天好友的uid为:"<<endl;
    cin>>recvuid;

    Message msg1(curuid,recvuid,CHAT_WITH_FRIEND);
    int ret = asocket.Send(msg1.S_to_json());
    err.server_close(ret);

    string recv=asocket.Receive();
    err.server_close(recv);
    if(recv=="notfind"){
        cout<<"好友列表中没有找到该好友"<<endl;
        return;
    }else if(recv=="success"){
        //打印历史聊天记录
        string historymsg;
        while(1){
            historymsg=asocket.Receive();
            err.server_close(historymsg);
            if(historymsg=="The end"){
                cout<<RED<<"输入:exit退出聊天,输入:S发送文件,输入:R接收文件"<<RESET<<endl;
                break;
            }else{
                cout<<historymsg<<endl;
            }
        }
        cin.ignore();
        //获取新聊天
        string newmsg;
        while(1){
            getline(cin,newmsg);
            if(newmsg==":exit"){
                //退出聊天
                Message msg_exit(curuid,recvuid,EXIT_CHAT);
                int ret=asocket.Send(msg_exit.S_to_json());
                err.server_close(ret);

                string recv=asocket.Receive();
                err.server_close(recv);
                if(recv=="success"){
                    cout<<"成功退出聊天"<<endl;
                    return;
                }else{
                    cout<<"出现错误"<<endl;
                    return;
                }
                break;
            }
            //发送文件
            if(newmsg==":S"){
            //获得文件的路径 打开文件 存储文件信息 提取文件名
                string filepath;//文件路径
                cout<<"请输入要发送文件的位置:"<<endl;
                getline(cin,filepath);
                int filefd=open(filepath.c_str(),O_RDONLY);
                if(filefd==-1){
                    cerr<<"Error opening file"<<endl;
                    return;
                }else{
                    struct stat statbuf;
                    fstat(filefd,&statbuf);//将与给定文件描述符关联的文件状态信息填充到statbuf结构体中
                    size_t lastSlash=filepath.find_last_of("/\\");//找到最后一个斜杠或者反斜杠
                    string filename=filepath.substr(lastSlash+1);//获取到文件名

                    Message msg_file(curuid,recvuid,SENDFILE,{filename,to_string(statbuf.st_size)});
                    int ret=asocket.Send(msg_file.S_to_json());
                    err.server_close(ret);

                    string recv_file=asocket.Receive();
                    err.server_close(recv);
                    if(recv_file=="success"){
                        ssize_t bytes_sent = 0;
                        cout<<statbuf.st_size<<endl;
                        while (bytes_sent < statbuf.st_size) {
                            ssize_t ret_send = sendfile(asocket.getfd(), filefd, &bytes_sent, statbuf.st_size - bytes_sent);
                            cout<<ret_send<<endl;
                            if (ret_send == -1){
                                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                                    // 出现错误继续尝试发送
                                    continue;
                                } else {
                                    cerr << "Error sending file data: " << strerror(errno) << endl;
                                    close(filefd);
                                    break;
                                }
                            } else if (ret_send == 0) {
                                cerr << "Connection closed by peer while sending file data." << endl;
                                break;
                            }
                        }
                    }
                    close(filefd);
                }

                string recv=asocket.Receive();
                err.server_close(recv);
                if(recv=="success"){
                    cout<<RED<<"文件上传成功"<<RESET<<endl;
                }
                continue;
            }
            //接收文件
            if(newmsg==":R")
            {
                string filepath;

                cout<<"请输入想保存文件的位置:"<<endl;
                getline(cin,filepath);

                size_t lastSlash=filepath.find_last_of("/\\");//找到最后一个斜杠或者反斜杠
                string filename=filepath.substr(lastSlash+1);//获取到文件名

                Message msg_file(curuid,recvuid,RECVFILE,{filename});
                int ret=asocket.Send(msg_file.S_to_json());
                err.server_close(ret);

                string recv_file=asocket.Receive();
                err.server_close(recv_file);
                if(recv_file=="failure"){
                    cout<<"暂时还没有未接收的文件"<<endl;
                    continue;
                }else{
                    int filefd=open(filepath.c_str(),O_APPEND|O_WRONLY|O_CREAT,S_IRWXU);
                    if(filefd==-1){
                        cerr<<"Error opening file:"<<strerror(errno)<<endl;
                        return;
                    }
                    cout<<recv_file.c_str()<<endl;
                    ssize_t size=stoll(recv_file.c_str());//文件大小
                    cout<<size<<endl;
                    char buf[BUFSIZ];//缓冲区
                    ssize_t totalRecvByte=0;

                    lseek(filefd,0,SEEK_SET);
                    while(size>totalRecvByte){
                        cout<<"size:"<<size<<endl;
                        ssize_t byteRead=read(asocket.getfd(),buf,sizeof(buf));
                         if (byteRead == -1) {
                            if(errno == EAGAIN || errno == EWOULDBLOCK){
                                continue;
                            }else{
                                cerr << "Error reading file: " << strerror(errno) << endl;
                            }
                        }
                        if (byteRead == 0) {
                            cerr << "Connection closed by client" << endl;
                            break;
                        }
                        if (size==totalRecvByte) {
                            cout<<"size:"<<size<<endl;
                            cout<<"totalRecvByte:"<<totalRecvByte<<endl;
                            //cerr << "Connection closed by client" << endl;
                            break;
                        }
                        ssize_t byteWritten=write(filefd,buf,byteRead);
                        if (byteWritten == -1) {
                            cerr << "Error writing to file" << endl;
                            break;
                        }
                        totalRecvByte+=byteRead;
                        cout<<"Curtotal:"<<totalRecvByte<<endl;
                        cout << "byteRead:" << byteRead << endl;
                        cout << "bytewrite" << byteWritten << endl;
                    }
                    if (totalRecvByte < size) {
                        cerr << "File transmission incomplete" << endl;
                    }
                    close(filefd);
                    cout<<RED<<"文件接收完毕"<<RESET<<endl;
                    continue;
                }
            }
            //发消息
            Message msg(curuid,recvuid,SENDMSG,{newmsg});
            int ret = asocket.Send(msg.S_to_json());
            err.server_close(ret);

            string recv=asocket.Receive();
            err.server_close(recv);
            if(recv=="failure"){
                return;
            }else if(recv=="success"){
                continue;
            }
        }
    }
    return ;

}