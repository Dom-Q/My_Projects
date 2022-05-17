#include "message.h"

Message::Message(){
    std::string data = "";
    std::string id = "";
    std::string HOST_NAME = "";
    double priority = 0.0;
    double priority_f = 0.0;
    msgtype msg_type = INIT;
}

Message::Message(std::string msg){
    int startidx = 0;
    int endidx = msg.find('/', startidx);
    data = msg.substr(0,endidx);

    startidx = endidx + 1;
    endidx = msg.find('/', startidx);
    id = msg.substr(startidx, endidx - startidx);

    startidx = endidx + 1;
    endidx = msg.find('/', startidx);
    HOST_NAME = msg.substr(startidx,endidx - startidx);

    startidx = endidx + 1;
    endidx = msg.find('/', startidx);
    bool is_dec = check_dec(msg.substr(startidx, endidx - startidx));
    if(is_dec){
        priority = std::stod(msg.substr(startidx, endidx - startidx));
    } 
    else{
        priority = 0;
    }

    startidx = endidx + 1;
    endidx = msg.find('/', startidx);
    is_dec = check_dec(msg.substr(startidx, endidx - startidx));
    if(is_dec){
        priority_f = std::stod(msg.substr(startidx, endidx - startidx));
    } 
    else{
        priority_f = 0;
    }

    startidx = endidx + 1;

    std::string m_type = msg.substr(startidx, msg.size() - startidx);

    if(m_type == "FINAL"){
        msg_type = FINAL;
    } else if(m_type == "RESP") {
        msg_type = RESP;
    } else{
        msg_type = INIT;
    }
}

Message::Message(std::string data_, std::string id_, std::string HOST_NAME_, double c_priority){
    data = data_;
    id = id_;
    HOST_NAME = HOST_NAME_;
    priority = c_priority;
    priority_f = 0.0;
    msg_type = INIT;
}

std::string Message::get_data(){
    return data;
}
std::string Message::get_id(){
    return id;
}
std::string Message::get_HOST_NAME(){
    return HOST_NAME;
}
msgtype Message::getType(){
    return msg_type;
}

double Message::getPriority(){
    return priority;
}

double Message::getPriority_f(){
    return priority_f;
}

void Message::set_data(std::string data_){
    data = data_;
}
void Message::set_id(std::string id_){
    id = id_;
}
void Message::set_HOST_NAME(std::string HOST_NAME_){
    HOST_NAME = HOST_NAME_;
}
void Message::set_Type(msgtype type_){
    msg_type = type_;
}

void Message::set_priority(double priority_){
    priority = priority_;

}
void Message::set_priority_f(double priority_){
    priority_f = priority_;
}

bool Message::check_dec(std::string input){
    bool dc_found = false;
    for(size_t i = 0; i < input.size(); i++){
        if(dc_found && input[i] == '.') return false;
        else if(!dc_found && input[i] == '.') dc_found = true;
        else if(input[i] < '0' || input[i] > '9'){
            return false;
        }
    }
    return true;
}

std::string Message::msgToString(){
    std::string output;
    output = data;
    output += "/";
    output += id;
    output += "/";
    output += HOST_NAME;
    output += "/";
    output += std::to_string(priority);
    output += "/";
    output += std::to_string(priority_f);
    output += "/";
    if(msg_type == INIT){
        output += "INIT";
    }else if(msg_type == RESP){
        output += "RESP";
    }else{
        output += "FINAL";
    }
    return output;
}
