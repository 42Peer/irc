#include "Command.hpp"
#include "userStruct.h"

void Message::run(int fd, std::vector<std::string> args) {
	// <@nick_name> message
	// @ : privilage
	
	struct s_user_info user_data;
	user_data = this->_handler.getServer().getUserData(fd);
	// if (user_name == "")
	// 	return ;
	user_data.channel_list.back();
	/*
		getUserfromChannel(channel_name) -> std::vector<int>
		while (it != v.end())
		{
			send((*it), args.first, sizeof(args.first), 0);

		}
	*/
}

void Notice::run(int fd, std::vector<std::string> args) {
	// <user or channel>{,<user or channel>} messsage
	// 
}
void Join::run(int fd, std::vector<std::string> args) {

}
void Nick::run(int fd, std::vector<std::string> args) {

}
void Quit::run(int fd, std::vector<std::string> args) {

}
void Privmsg::run(int fd, std::vector<std::string> args) {

}
void Kick::run(int fd, std::vector<std::string> args) {

}
void Part::run(int fd, std::vector<std::string> args) {

}
void List::run(int fd, std::vector<std::string> args) {

}