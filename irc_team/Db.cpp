#include "Db.hpp"
#include <exception>

int main() {
    Db db;
    s_user_info cuser;
	s_user_info user2;
	s_user_info jujeon;

	jujeon.privileges = -1;
	jujeon.nick = "jujeon";
	jujeon.name = "joon";
	jujeon.fd = 13;
    cuser.privileges = -1;
    cuser.nick = "cuser";
    cuser.name = "bye";
    cuser.fd = 13;
	user2.nick = "user";
	user2.name = "bye";
	user2.fd = 12;
	user2.privileges = 0;
	// db.addUser(jujeon);
    // db.addChannel("hello channel!");
    // db.addUser(cuser);
    // UserData& user = db.getUserTable();
    // user.printTables();
	// db.addChannelUser(cuser, "hello");
	// db.printChannelTables();
	// db.addChannelUser(user2, "hello");
	db.addChannelUser(user2, "hello2");
	std::cout << db.convertFdToId(13) << std::endl;
	// db.addChannelUserFd(12, "hello");
	// db.removeChannel(user2, "hello");
	// db.removeChannel(jujeon, "hello");
	// db.removeChannel(jujeon, "test");
	// std::cout << db.updateUser(jujeon, cuser) << std::endl;
    // db.addChannel("helo");
    // db.getCorrectChannel("helo").addData(0, cuser);
    // ChannelData& channel = db.getCorrectChannel("hello channel!");
    // channel.addData(0, cuser);
	// user.printTables();
	// std::cout << "print before change user *\n";
    // db.printChannelTables();
	// db.updateUser(cuser, user2);
	// std::cout << "print after change user *\n";
	// db.printChannelTables();
    // std::cout << "done \n";
//    db.removeChannel("hello channel", cuser);
//    db.removeChannel("hello channel!", user2);
	// db.getUserTable().printTables();
    // db.removeUser(user2);
	db.printChannelTables();
	// std::cout << "-----print user-----\n";
	db.printUserTables();
	// user.printTables();
//    std::cout << "user :" << std::endl;
//    user.printTables();
//    std::cout << "....\n";
    // db.printTables();
}
