#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<hiredis/hiredis.h>
#define SERVERPORT 8080
using namespace std;
int client_fd;
sockaddr_in server_addr;