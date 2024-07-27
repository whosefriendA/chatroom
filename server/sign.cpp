#include"server.hpp"
void Sign_up(TaskSocket asocket,json command){
    string a=command.at("name");
    string b=command.at("pass");
    string c=command.at("question");
    string d=command.at("answer");
    User u(a,b,c,d);
    asocket.Sendmsg()

}
void Log_in(TaskSocket asocket,json command);
void Log_out(TaskSocket asocket,json command);