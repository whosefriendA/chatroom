#include<iostream>
#include<string>
#include<vector>
#include<nlohmann/json.hpp>

using namespace std;
using json=nlohmann::json;

class Message{
public:
    Message()=default;
    ~Message()=default;
    Message(string uid,string question,string recvuid,int flag,vector<string> option)
    : uid(uid),question(question),recvuid(recvuid), flag(flag), option(option){}
    string uid,name,pass,recvuid,question,answer;
    int flag;
    vector<string> option;
    void Json_to_s(string jsr){
        try {
            json js = json::parse(jsr);
            js.at("uid").get_to(uid);
            js.at("name").get_to(name);
            js.at("pass").get_to(name);
            js.at("question").get_to(question);
            js.at("answer").get_to(answer);
            js.at("recvuid").get_to(recvuid);
            js.at("flag").get_to(flag);
            js.at("option").get_to(option);
        } catch (const exception& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
        }
    }
    string S_to_json(){
        json js=json{
            {"uid",uid},
            {"name",name},
            {"pass",pass},
            {"question",question},
            {"answer",answer},
            {"recvuid",recvuid},
            {"flag",flag},
            {"option",option},
        };
        return js.dump();
    }
};