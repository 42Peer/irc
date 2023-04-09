#include "Db.hpp"
#include <exception>

int main() {
    Db db;
    s_user_info cuser;
	s_user_info user2;

    cuser.privileges = 0;
    cuser.nick = "nick";
    cuser.name = "bye";
    cuser.fd = 12;
	user2.nick = "name";
	user2.name = "bye";
	user2.fd = 12;
	user2.privileges = 0;
    db.addChannel("hello channel!");
    db.addUser(cuser);
    UserData& user = db.getUserTable();
    // user.printTables();
    db.addChannel("helo");
    db.getCorrectChannel("helo").addData(0, cuser);
    ChannelData& channel = db.getCorrectChannel("hello channel!");
    channel.addData(0, cuser);
	// user.printTables();
	std::cout << "print before change user *\n";
    db.printChannelTables();
	db.updateUser(cuser, user2);
	std::cout << "print after change user *\n";
	db.printChannelTables();
    // std::cout << "done \n";
   db.removeChannel("hello channel", cuser);
   db.removeChannel("hello channel!", user2);
    db.removeUser(user2);
	std::cout << "-----print user-----\n";
	user.printTables();
//    std::cout << "user :" << std::endl;
//    user.printTables();
//    std::cout << "....\n";
    // db.printChannelTables();
}
