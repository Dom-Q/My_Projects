#include <string>
#include <iostream>

enum msgtype {INIT, RESP, FINAL}; 

class Message {
    private:
        std::string data;
        std::string id;
        std::string HOST_NAME;
        double priority;
        double priority_f;
        msgtype msg_type;
    public:
        std::string get_data();
        std::string get_id();
        std::string get_HOST_NAME();
        msgtype getType();
        double getPriority();
        double getPriority_f();

        void set_data(std::string data_);
        void set_id(std::string id_);
        void set_HOST_NAME(std::string HOST_NAME_);
        void set_Type(msgtype type_);
        void set_priority(double priority_);
        void set_priority_f(double priority_);
        std::string msgToString();
        Message();
        Message(std::string msg);
        Message(std::string data_, std::string id_, std::string HOST_NAME_, double c_priority);

        bool check_dec(std::string input);
};
