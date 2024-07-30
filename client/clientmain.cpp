#include"./client.hpp"
#include"../TaskSocket.hpp"
int main(int argc,char*argv[]){
    // struct termios term;
    // tcgetattr(0,&term);
    // term.c_cc[VEOF]=0;
    // tcsetattr(0,TCSANOW,&term);
    
    server_addr2.sin_family=AF_INET;
    server_addr2.sin_port=SERVERPORT;
    server_addr2.sin_addr.s_addr=INADDR_ANY;
     int opt;
    while((opt=getopt(argc,argv,"i:p:"))!=-1){
        switch(opt){
        case 'i':
        server_addr2.sin_addr.s_addr=inet_addr(optarg);
        break;
        case 'p':
        server_addr2.sin_port=std::stoi(optarg);
        break;
        }
    }
    int client_fd=socket(AF_INET,SOCK_STREAM,0);
    if(client_fd==-1){
        perror("client socket error");
        exit(0);
    }
    TaskSocket asocket(client_fd);
    if(connect(client_fd,(sockaddr *)&server_addr2,sizeof(server_addr2))){
        perror("connect error");
        exit(0);
    }
    int ret=Main_Menu();
    if(ret==1){
        
    }
}