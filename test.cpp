#include<iostream>
#include<string>
#include<cstdlib>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/types.h>
#include<hiredis/hiredis.h>
#include<mutex>
#include<json/json.h>
using namespace std;
