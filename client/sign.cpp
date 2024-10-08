#include "./client.hpp"
void sign_up()
{
    string name, pass, pass2;
    string question, answer;
    cout << "请输入昵称:" << endl;
    getline(cin, name);
    while (1)
    {
        cout << "请输入密码:" << endl;
        getline(cin, pass);
        while (pass.size() == 0)
        {
            cout << "请重新输入有效的密码:" << endl;
            getline(cin, pass);
        }
        cout << "请确认密码:" << endl;
        getline(cin, pass2);
        if (pass != pass2)
        {
            system("clear");
            cout << "密码不一致" << endl;
        }
        else
            break;
    }
    cout << "设置密保问题" << endl;
    getline(cin, question);
    cout << "设置答案" << endl;
    getline(cin, answer);
    Message msg(name, question, SIGNUP, pass, {answer});
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);

    string uid = asocket.Receive_client(); // 收到生成的uid
    if (uid == "close")
    {
        cout << "已关闭" << endl;
        exit(0);
    }
    cout << "你注册的uid为:" << uid << endl;
    cout << "请登录" << endl;

    return;
}
string get_uid()
{
    string uid;
    int flag = 1, c;

    cout << "你的uid为:" << endl;
    getline(cin, uid); // 获取输入的uid
    for (int c : uid)
    {
        if (!isdigit(c))
        {
            flag = 0;
            break;
        }
    }
    while (flag == 0)
    {
        cout << "输入的uid有非法字符,请重新输入:" << endl;
        getline(cin, uid);
        flag = 1;
        for (int c : uid)
        {
            if (!isdigit(c))
            {
                flag = 0;
                break;
            }
        }
    }
    return uid;
}
void *notify_receive(void *arg)
{
    struct ThreadParams
    {
        std::string uid;
        int recv_fd;
    };
    ThreadParams *params = static_cast<ThreadParams *>(arg);
    TaskSocket recvsocket(params->recv_fd);
    if (connect(params->recv_fd, (sockaddr *)&server_addr2, sizeof(server_addr2)))
    {
        perror("connect error");
        exit(0);
    }
    Message msg(params->uid, -1);
    int ret = recvsocket.Send(msg.S_to_json());
    if (ret == -1 || ret == 0)
    {
        std::cout << "服务器已关闭" << std::endl;
        delete params;
        exit(0);
    }
    while (true)
    {
        std::string recv = recvsocket.Receive_client();
        if (recv == "close")
        {
            std::cout << "服务器已关闭" << std::endl;
            delete params;
            exit(0);
        }
        std::cout << recv << std::endl;
    }
    return nullptr;
}
int log_in()
{
    string uid, pass;
    while (1)
    {
        uid = get_uid();
        // cout<<"uid="<<uid<<endl;
        curuid = uid;
        cout << "请输入你的密码:" << endl;
        getline(cin, pass);
        // cout<<pass<<endl;
        Message msg(uid, LOGIN, {pass});
        int ret = asocket.Send(msg.S_to_json());
        // err.server_close(ret);
        string recv = asocket.Receive_client(); // 接收返回的结果
        cout << recv << endl;
        err.server_close(recv);
        if (recv == "notcorrect")
        {
            cout << "密码错误：" << endl;
            continue;
        }
        else if (recv == "havenotsignup")
        {
            cout << "帐号不存在,请注册" << endl;
            continue;
        }
        else if (recv == "again")
        {
            cout << "这个帐号已经登录了，请勿重复登录" << endl;
            continue;
        }
        else
        {
            curname = recv;
            system("clear");
            cout << "登录成功" << endl;
            pthread_t tid;
            // 额外创建线程处理通知
            struct ThreadParams
            {
                string uid;
                int recv_fd;
            };
            ThreadParams *param = new ThreadParams{curuid, asocket.get_recvfd()};
            pthread_create(&tid, NULL, &notify_receive, static_cast<void *>(param));
            pthread_detach(tid);
            return 1;
        }
        return 1;
    }
}
void pass_find()
{
    string uid, pass, answer;
    uid = get_uid();
    Message msg(uid, QUESTION_GET);
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv = asocket.Receive_client();
    err.server_close(recv);
    cout << recv << endl;
    getline(cin, answer);
    Message msg1(uid, PASSWORD_FIND);
    ret = asocket.Send(msg1.S_to_json());
    err.server_close(ret);

    recv = asocket.Receive_client(); // 接收返回的结果
    err.server_close(recv);
    if (recv != answer)
    {
        cout << "答案不正确,无法找回" << endl;
        return;
    }
    else if (recv == answer)
    {
        Message msg2(uid, PASSWORD_GET);
        ret = asocket.Send(msg2.S_to_json());
        err.server_close(ret);

        recv = asocket.Receive_client();
        cout << "你的密码是：" << recv << "千万别再忘了" << endl;
        return;
    }
}
void Unreadnotice()
{
    Message msg(curuid, UNREAD_NOTICE);
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv = asocket.Receive_client();
    err.server_close(recv);
    if (recv == "failure")
    {
        cout << "你目前没有未读消息" << endl;
    }
    else
    {
        cout << recv << endl;
        return;
    }
}
int user_remove()
{
    Message msg(curuid, USER_REMOVE);
    int ret = asocket.Send(msg.S_to_json());
    err.server_close(ret);
    string recv = asocket.Receive_client();
    err.server_close(recv);
    if (recv == "success")
    {
        cout << "注销成功" << endl;
        return 1;
    }
    return 0;
}