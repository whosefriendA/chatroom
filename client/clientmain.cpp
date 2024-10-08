#include "./client.hpp"
sockaddr_in server_addr2;
TaskSocket asocket("receive");
Error err;

string curuid, curname, cur_groupuid;
int main(int argc, char *argv[])
{
    server_addr2.sin_family = AF_INET;
    server_addr2.sin_port = htons(SERVERPORT);
    server_addr2.sin_addr.s_addr = htonl(INADDR_ANY);
    int opt;
    while ((opt = getopt(argc, argv, "i:p:")) != -1)
    {
        switch (opt)
        {
        case 'i':
            server_addr2.sin_addr.s_addr = inet_addr(optarg);
            break;
        case 'p':
            server_addr2.sin_port = stoi(optarg);
            break;
        }
    }
    // cout<<server_addr2.sin_addr.s_addr<<endl;
    // cout<<server_addr2.sin_port<<endl;
    if (connect(asocket.getfd(), (struct sockaddr *)&server_addr2, sizeof(server_addr2)) == -1)
    {
        perror("connect error");
        exit(0);
    }
    struct termios term;
    tcgetattr(0, &term);          // 获取终端属性
    term.c_cc[VEOF] = 0;          // 禁用 EOF 键
    tcsetattr(0, TCSANOW, &term); // 设置新的终端属性
    signal(SIGINT, SIG_IGN);      // ctrl+c
    // signal(SIGQUIT, SIG_IGN);     // ctrl+"\"
    signal(SIGTSTP, SIG_IGN);     // ctrl+z
    int ret = Main_menu();
    if (ret == 1)
    {
        User_menu();
    }
}
