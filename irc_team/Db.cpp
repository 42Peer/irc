#include "Db.hpp"
#include <exception>

int main() {
    Db db;
    s_user_info cuser;

    cuser.privileges = 0;
    cuser.nick = "nick";
    cuser.name = "bye";
    cuser.fd = 12;
    db.addChannel("hello channel!");
    db.addUser(cuser);
    UserData& user = db.getUserTable();
    user.printTables();

    ChannelData& channel = db.getCorrectChannel("hello channel!");
    channel.addData(0, cuser);
//    channel.removeData(cuser.nick);
    db.printChannelTables();
    db.removeChannel("hello channel!", cuser);
//    ChannelData channel;
//    if (channel.addData(0, 12, "hi", "bye") == false) {
//        std::cout << "Error occur in add Data in channel" << std::endl;
//    }
//    if (channel.addData(1, 12, "hi2", "bye") == false) {
//        std::cout << "Error occur in add Data in channel" << std::endl;
//    }
//    if (channel.addData(1, 13, "hi", "good") == false) {
//        std::cout << "Error occur in add Data in channel" << std::endl;
//    }
//    channel.printTables();
    db.printChannelTables();
}
