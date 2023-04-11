#include "Command.hpp"
#include "userStruct.h"

void Message::run(int fd, std::vector<std::string> args) {
	struct s_user_info user_data;
	std::string name;
	name = this->_handler.getServer().getUserName(fd);
	user_data = g_db.getUserTable().getUser(name);
	if (!user_data.channel_list.size())
		return ;
	std::vector<std::string>::iterator it = this->_handler.getServer().getChannelRef().getUserList(user_data.channel_list.back()).begin();
	std::vector<std::string>::iterator eit = this->_handler.getServer().getChannelRef().getUserList(user_data.channel_list.back()).end();
	while (it != eit)
	{
		send(g_db.getUserTable().getUser(*it).fd, args.front().c_str(), sizeof(args.front().c_str()), 0);
		++it;
	}
}

void Notice::run(int fd, std::vector<std::string> args) {
	// <user or channel>{,<user or channel>} messsage
	// 
}
void Join::run(int fd, std::vector<std::string> args) {
/*
    "/join [채널명]" : 특정 채널에 조인합니다. (확인필요)
*/
	std::vector<std::string>::iterator it = args.begin();
	std::string name = this->_handler.getServer().getUserName(fd);
	while (it != args.end())
	{
		this->_handler.getServer().getChannelRef().setList(*it, name);
		++it;
	}
}

void Nick::run(int fd, std::vector<std::string> args) {
	std::string old_name = this->_handler.getServer().getUserName(fd);
	std::string new_name = args.front();
	struct s_user_info old_user_info = g_db.getUserTable().getUser(old_name);
	struct s_user_info new_user_info;

	new_user_info.nick = new_name;
	new_user_info.name = old_user_info.name;
	new_user_info.fd = old_user_info.fd;
	new_user_info.channel_list = old_user_info.channel_list;

	if (!g_db.updateUser(old_user_info, new_user_info))
	{
		//error_sending
		// const char* msg = "Duplicated nick";
		// send(fd, msg, strlen(msg), 0);
		/*
			에러넘버를 세팅할지 말지 (논의 필요)
		*/
		return ;
	}
	std::vector<std::string>::iterator it = g_db.getUserTable().getUser(new_name).channel_list.begin();
	std::vector<std::string>::iterator eit = g_db.getUserTable().getUser(new_name).channel_list.end();
	while (it != eit)
	{
		this->_handler.getServer().getChannelRef().changeNameFromChannelList(*it, old_name, new_name);
		++it;
	}
	this->_handler.getServer().setMapData(fd, new_name);
}

void Quit::run(int fd, std::vector<std::string> args) {
	std::string usr_name = this->_handler.getServer().getUserName(fd);
	struct s_user_info usr_name_info = g_db.getUserTable().getUser(usr_name);
	std::vector<std::string>::iterator it = usr_name_info.channel_list.begin();
	while(it != usr_name_info.channel_list.end())
	{
		this->_handler.getServer().getChannelRef().removeUserFromChannel(*it, usr_name);
		++it;
	} 
	g_db.removeUser(usr_name_info);
	this->_handler.getServer().removeMapData(fd);
	close(fd);
}

void Privmsg::run(int fd, std::vector<std::string> args) {
	std::vector<std::string>::iterator it = args.begin();

	while(it != args.end() - 1)
	{
		if(g_db.getUserTable().isExist(*it))	
			send(g_db.getUserTable().getUser(*it).fd, args.back().c_str(), strlen(args.back().c_str()), 0); 
		// else /* error msg */
			// send(fd, "Erro")
		++it;
	}
}

void Kick::run(int fd, std::vector<std::string> args) {
	std::string name = this->_handler.getServer().getUserName(fd);
	if (g_db.getUserTable().getUser(name).channel_list.size() <= 0)
		return ; /* error msg */
	std::string kicked_name = args.front();
	std::string current_channel = g_db.getUserTable().getUser(name).channel_list.back();
	if (!g_db.getUserTable().isExist(kicked_name))
		return ; /* error msg */
	else if(!(this->_handler.getServer().getChannelRef().isExistInChannel(current_channel, kicked_name)))
		return ; /* error msg */
	if (g_db.getCorrectChannel(g_db.getUserTable().getUser(name).channel_list.back()).getPrivileges(name) != -1)
	{
		g_db.getUserTable().removeChannel(g_db.getUserTable().getUser(kicked_name), current_channel);
		this->_handler.getServer().getChannelRef().removeUserFromChannel(current_channel, kicked_name);
		// send(g_db.getUserTable().getUser(kicked_name).fd,) /* send kicked msg */
	}
	else
		return ; /* error msg */
}

void Part::run(int fd, std::vector<std::string> args) {
	std::string name = this->_handler.getServer().getUserName(fd);

	if (!g_db.getUserTable().getUser(name).channel_list.size())
		return ; /* error msg */
	else if (!args.size())
	{
		std::string current_channel = g_db.getUserTable().getUser(name).channel_list.back();
		g_db.getUserTable().removeChannel(g_db.getUserTable().getUser(name), current_channel);
	}
	else
	{
		std::vector<std::string>::iterator it = args.begin();
		while (it != args.end())
		{
			std::vector<std::string>::iterator fit = g_db.getUserTable().getUser(name).channel_list.begin();
			std::vector<std::string>::iterator feit = g_db.getUserTable().getUser(name).channel_list.end();
			std::vector<std::string>::iterator fpos = std::find(fit, feit, *it);
			if (fpos != feit)
				g_db.getUserTable().removeChannel(g_db.getUserTable().getUser(name), *it);
			else
			{
				; /* send err msg */
			}
			++it;
		}
	}
}
