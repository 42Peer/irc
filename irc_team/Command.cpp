#include "Command.hpp"

void Message::run(int fd, std::vector<std::string> args) {
	// <@nick_name> message
	// @ : privilage
	/*
		getChannel(fd);
		-->current Channel
		if (!curret_channel)
			return ;
		while (user_list)
			send(channel->userlist->channel_fd, args.first, sizeof(args.first), 0);	
	*/
	Db db;
	struct s_user_data user_name;
	user_name = this->_handler.getServer().getUserData(fd);
	// if (user_name == "")
	// 	return ;
	else
	{
	}
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