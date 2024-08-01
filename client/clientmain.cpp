#include"./client.hpp"
sockaddr_in server_addr2;
TaskSocket asocket;
int main(int argc,char*argv[]){
    struct termios term;
    tcgetattr(0,&term);
    term.c_cc[VEOF]=0;
    tcsetattr(0,TCSANOW,&term);
    server_addr2.sin_family=AF_INET;
    server_addr2.sin_port=htons(SERVERPORT);
    server_addr2.sin_addr.s_addr=htonl(INADDR_ANY);
    //  int opt;
    // while((opt=getopt(argc,argv,"i:p:"))!=-1){
    //     switch(opt){
    //     case 'i':
    //     server_addr2.sin_addr.s_addr=inet_addr(optarg);
    //     break;
    //     case 'p':
    //     server_addr2.sin_port=std::stoi(optarg);
    //     break;
    //     }
    // }
    int client_fd=asocket.tfd;
    if(connect(client_fd,(struct sockaddr *)&server_addr2,sizeof(server_addr2))==-1){
        perror("connect error");
        exit(0);
    }
    int ret=Main_menu();
    if(ret==1){
        User_menu();
    }
}