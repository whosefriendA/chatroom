#include"client.hpp"
int main(int argc,char*argv[]){
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=SERVERPORT;
    server_addr.sin_addr.s_addr=INADDR_ANY;
     int opt;
    while(opt=getopt(argc,argv,"i:p:")!=-1){
        switch(opt){
        case 'i:':
        server_addr.sin_addr.s_addr=inet_addr(optarg);
        case 'p:':
        server_addr.sin_port=std::stoi(optarg);
        }
    }
    if(client_fd=socket(AF_INET,SOCK_STREAM,0)==-1){
        perror("client socket error");
        exit(0);
    }
    if(connect(client_fd,(sockaddr *)&server_addr,sizeof(server_addr))){
        perror("connect error");
        exit(0);
    }
    
}