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
    db.addChannel("helo");
    db.getCorrectChannel("helo").addData(0, cuser);
    ChannelData& channel = db.getCorrectChannel("hello channel!");
    channel.addData(0, cuser);
    db.printChannelTables();
    std::cout << "done \n";
//    db.removeChannel("hello channel", cuser);
//    db.removeChannel("hello channel!", cuser);
    db.removeUser(cuser);
//    std::cout << "user :" << std::endl;
//    user.printTables();
//    std::cout << "....\n";
    db.printChannelTables();
}
