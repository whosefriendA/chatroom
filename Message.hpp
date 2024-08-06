#include<iostream>
#include<string>
#include<vector>
#include<nlohmann/json.hpp>

using namespace std;
using json=nlohmann::json;

class Message{
public:
    Message()=default;
    Message(string uid,int flag):uid(uid),flag(flag){};
    Message(string uid,int flag,vector<string>option):uid(uid),flag(flag),para(option){};
    Message(string uid,string recuid,int flag):uid(uid),recuid(recuid),flag(flag){};
    Message(string uid,string recuid,int flag,vector<string>option):uid(uid),recuid(recuid),flag(flag),para(option){};
    Message(string name,string question,int flag,string pass,vector<string>option):name(name),question(question),flag(flag),pass(pass),para(option){};
    Message(string uid,string question,string recuid,int flag,string name,string pass,vector<string> option)
    : uid(uid),question(question),recuid(recuid), flag(flag), name(name),pass(pass),para(option){}
    ~Message()=default;
    string uid,name,pass,recuid,question;
    int flag;
    vector<string> para;
    void Json_to_s(string jsr){
        try {
            json js = json::parse(jsr);
            js.at("uid").get_to(uid);
            js.at("name").get_to(name);
            js.at("pass").get_to(name);
            js.at("question").get_to(question);
            js.at("recuid").get_to(recuid);
            js.at("flag").get_to(flag);
            js.at("option").get_to(para);
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
            {"recuid",recuid},
            {"flag",flag},
            {"option",para},
        };
        return js.dump();
    }
};