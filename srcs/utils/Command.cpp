#include "Command.hpp"
#include "Db.hpp"
#include "userStruct.hpp"
#include "Message.hpp"

bool existInVector(std::vector<std::string> &vec, std::string value) {
	std::vector<std::string>::iterator it = vec.begin();
	for (; it != vec.end(); ++it) {
		if (*it == value)
			return (true);
	}
	return (false);
}

void Notice::run(int fd, std::vector<std::string> args) {
	std::string buf("");
	std::string my_name = this->_handler.getServer().getUserName(fd);
	if (args.front()[0] == '#') {
		buf.append(":");
		buf.append(SERVNAME);
		buf.append(ERR404 + my_name + " " + args[0] + MSG404);
		this->_handler.getServer().setFdMessage(fd, buf);
	}
	else if (this->_handler.getServer().g_db.getUserTable().isExist(args.front()) == false) {
		buf.append(":");
		buf.append(SERVNAME);
		buf.append(ERR401 + my_name + " " + args[0] + MSG401);
		this->_handler.getServer().setFdMessage(fd, buf);
	}
	else {
		buf.append(":" + my_name + " NOTICE " + args.front() + " :" + args.back() + "\r\n");
		int receiver = this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd;
		this->_handler.getServer().setFdMessage(receiver, buf);
	}
}


void Join::run(int fd, std::vector<std::string> args) {
	std::string nick_name(this->_handler.getServer().getUserName(fd));
	std::string buf("");

	for (size_t i = 0; i < args.size(); ++i) {
		if ((args[i][0] != '#' && args[i][0] != '&') || args[i].find(0x07) != std::string::npos) {
			buf = ":";
			buf += SERVNAME;
			buf += ERR476 + nick_name + " " + args[i] + MSG476;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue;
		}
		if (this->_handler.getServer().g_db.getUserTable().getUser(nick_name).channel_list.size() > 0
				&& this->_handler.getServer().g_db.getUserTable().getUser(nick_name).channel_list.back() == args[i]) {
			continue;
		}
		else {
			struct s_user_info& curr_user = this->_handler.getServer().g_db.getUserTable().getUser(nick_name);
			this->_handler.getServer().g_db.addChannelUser(curr_user, args[i]);
			ChannelData chn = this->_handler.getServer().g_db.getCorrectChannel(args[i]);

			std::vector<std::string> user_list = chn.getUserList();
			std::string king("");
			for (size_t h = 0; h < user_list.size(); ++h) {
				if (this->_handler.getServer().g_db.getCorrectChannel(args[i]).getPrivileges(user_list[h]) == 0) {
					king = user_list[h];
					break ;
				}
			}

			buf.append(":");
			buf.append(nick_name);
			buf.append(" JOIN ");
			buf.append(args[i]);
			buf.append("\r\n");

			buf.append(":");
			buf.append(SERVNAME);
			buf.append("353 ");

			buf.append(nick_name);
			buf.append(" = ");
			buf.append(args[i]);
			buf.append(" :@");
			buf.append(king);
			std::vector<std::string> chan_user_list = _handler.getServer().g_db.getCorrectChannel(args[i]).getUserList();
			for (unsigned long i = 0; i < chan_user_list.size(); i++)
			{
				if (chan_user_list[i] != king)
				{
					buf.append(" ");
					buf.append(chan_user_list[i]);
				}
			}

			buf.append("\r\n");

			buf.append(":");
			buf.append(SERVNAME);
			buf.append("366 ");
			buf.append(nick_name);
			buf.append(" ");
			buf.append(args[i]);
			buf.append(" :End of /NAMES list.\r\n");

			int receiver(0);
			for (size_t j = 0; j < user_list.size(); ++j) {
				receiver = this->_handler.getServer().g_db.getUserTable().getUser(user_list[j]).fd;
				this->_handler.getServer().setFdMessage(receiver, buf);
			}
			buf.clear();
		}
	}
}


void Nick::run(int fd, std::vector<std::string> args) {
	std::string buf("");
	std::string my_name = this->_handler.getServer().getUserName(fd);
	std::string new_nick = args[0];
	std::string current_nick(this->_handler.getServer().getUserName(fd));
	if (current_nick == "")
		current_nick = "*";

	if (!isValidName(new_nick)) {
		buf.append(":");
		buf.append(SERVNAME);
		buf.append(ERR432);
		buf.append(current_nick + " " + new_nick + MSG432);
		this->_handler.getServer().setFdMessage(fd, buf);
		return;
	}
	if (this->_handler.getServer().g_db.getUserTable().isExist(new_nick)) {
		buf.append(":");
		buf.append(SERVNAME);
		buf.append(ERR433);
		buf.append(current_nick + " " + new_nick + MSG433);
		this->_handler.getServer().setFdMessage(fd, buf);
		return;
	} 

	s_user_info old_user_info = this->_handler.getServer().g_db.getUserTable().getUser(current_nick);
	if (this->_handler.getServer().getFdFlagsStatus(fd, 0))
	{
		if (!this->_handler.getServer().getFdFlagsStatus(fd, 1))
		{
			struct s_user_info new_client;
			new_client.fd = fd;
			new_client.nick = new_nick;
			new_client.name = "*";
			new_client.real = "*";
			this->_handler.getServer().g_db.getUserTable().addUser(new_client);
			old_user_info = this->_handler.getServer().g_db.getUserTable().getUser(new_nick);
			this->_handler.getServer().setFdFlagsOn(fd, 1);
		}

		struct s_user_info new_user_info;
		new_user_info.nick = new_nick;
		new_user_info.name = old_user_info.name;
		new_user_info.real = old_user_info.real;
		new_user_info.fd = old_user_info.fd;
		new_user_info.channel_list = old_user_info.channel_list;

		this->_handler.getServer().g_db.updateUser(old_user_info, new_user_info);

		buf.append(":");
		buf.append(current_nick + " NICK " + new_nick + "\r\n");
		this->_handler.getServer().setFdMessage(fd, buf);
	}

	this->_handler.getServer().setMapData(fd, new_nick);
	if (this->_handler.getServer().checkGreetingMessage(fd)){
		this->_handler.getServer().setFdMessage(fd, RPL001 + new_nick +  MSG001);
		this->_handler.getServer().setFdFlagsOn(fd, 3);
	}
}

bool Nick::isValidName(const std::string& name) {
	// https://modern.ircdocs.horse/#clients
	if (name.find(' ') != std::string::npos ||
	name.find(',') != std::string::npos ||
	name.find('*') != std::string::npos ||
	name.find('?') != std::string::npos ||
	name.find('!') != std::string::npos ||
	name.find('@') != std::string::npos ||
	name.find('.') != std::string::npos ||
	name[0] == '$' || name[0] == ':' || name[0] == '#' || name[0] == '&' || name[0] == '_')
		return false;
	return true;
}


void Quit::run(int fd, std::vector<std::string> args) {
	(void) args;
	std::string usr_name = this->_handler.getServer().getUserName(fd);
	struct s_user_info usr_name_info = this->_handler.getServer().g_db.getUserTable().getUser(usr_name);
	std::vector<std::string> chn_list = usr_name_info.channel_list;
	for (size_t index = 0; index < chn_list.size(); ++index){
		ChannelData& chn = this->_handler.getServer().g_db.getCorrectChannel(chn_list[index]);
		if (chn.getUserList().size() == 0) {
				_handler.getServer().setFdMessage(fd, ERR403);
		} else {
			struct s_user_info user_info = this->_handler.getServer().g_db.getUserTable().getUser(usr_name);
			std::vector<std::string> channel_user = chn.getUserList();
			
			chn.removeData(usr_name);
			this->_handler.getServer().g_db.getUserTable().removeChannel(user_info, chn_list[index]);
			std::string buf("");
			if (args.size() == 0)
				buf += ":" + usr_name + " QUIT :Client exited\r\n";
			else
				buf += ":" + usr_name + " QUIT :Quit: " + args[0] + "\r\n";
	
			std::vector<std::string> user_list = chn.getUserList();
			int receiver = 0;
			for (size_t j = 0; j < user_list.size(); ++j) {
				receiver = this->_handler.getServer().g_db.getUserTable().getUser(user_list[j]).fd;
				this->_handler.getServer().setFdMessage(receiver, buf);
			}
		}
	}
	std::string buf("");
	if (!args.empty())
		buf = "ERROR :Closing link: [QUIT:" + args[0] + "]\r\n";
	else
		buf = "ERROR :Closing link: [QUIT: Client exited]\r\n";
	this->_handler.getServer().setFdMessage(fd, buf);
	this->_handler.getServer().g_db.removeUser(usr_name_info);
	this->_handler.getServer().removeMapData(fd);
	this->_handler.getServer().setFdFlagsOn(fd, 4);
}

std::vector<bool> Privmsg::checkduplicatedArgs(std::vector<std::string>& args) {
	std::map<std::string, int> check;
	std::vector<bool> ret_arg;
	ret_arg.resize(args.size() - 1);
	for (size_t i = 0; i < args.size() - 1; ++i) {
		int tmp = check[args[i]];
		if (tmp != 0) {
			ret_arg[i] = true;
		} else {
			ret_arg[i] = false;
		}
		check[args[i]] = tmp + 1;
	}
	return (ret_arg);
}

void Privmsg::run(int fd, std::vector<std::string> args) {
	std::vector<bool> res_args = checkduplicatedArgs(args);
	std::vector<std::string> users;
	UserData user = this->_handler.getServer().g_db.getUserTable();
	std::string msg;
	std::string buf;
	std::string my_name = this->_handler.getServer().getUserName(fd);
	msg = ":" + my_name + " PRIVMSG ";
	for (size_t i = 0; i < args.size() - 1; ++i) {
		if (!user.isExist(args[i])) {
			if (args[i][0] == '#' || args[i][0] == '&') {
				ChannelData chn = this->_handler.getServer().g_db.getCorrectChannel(args[i]);
				if (!chn.findUser(this->_handler.getServer().getUserName(fd))){
					buf = ":";
					buf += SERVNAME;
					buf += ERR442 + my_name + " " + args[i] + MSG442;
					this->_handler.getServer().setFdMessage(fd, buf);
					continue;
				}
				if (res_args[i] == true) {
					buf = ":";
					buf += SERVNAME;
					buf += ERR407 + my_name + " " + args[i] + MSG407;
					this->_handler.getServer().setFdMessage(fd, buf);
					continue;
				}
				std::vector<std::string> user_lists = chn.getUserList();
				for (size_t j = 0; j < user_lists.size(); ++j) {
					if (fd != user.getUser(user_lists[j]).fd)
						this->_handler.getServer().setFdMessage(
							user.getUser(user_lists[j]).fd, msg + args[i] + " :" + args.back() + "\r\n");
				}
				continue;
			}
			buf = ":";
			buf += SERVNAME;
			buf += ERR401 + my_name + " " + args[i] + MSG401;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue;
		}
		if (res_args[i] == true) {
			buf = ":";
			buf += SERVNAME;
			buf += ERR407 + my_name + " " + args[i] + MSG407;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue;
		}
		struct s_user_info cur_user = user.getUser(args[i]);
		this->_handler.getServer().setFdMessage(cur_user.fd, msg + cur_user.nick + " :" + args.back() + ";\r\n");
	}
}

void Kick::run(int fd, std::vector<std::string> args)
{
	std::string name = this->_handler.getServer().getUserName(fd);
	std::string message("");
	std::string buf("");
	std::vector<std::string> channels = splitByComma(args[0]);
	std::vector<std::string> targets = splitByComma(args[1]);
	struct s_user_info target_info;

	if (args.size() == 3)
		message = args.back();
	for(size_t i = 0; i < channels.size(); ++i){
		ChannelData channel_data = _handler.getServer().g_db.getCorrectChannel(channels[i]);
		if ((channels[i][0] != '#' && channels[i][0] != '&') || channels[i].find(0x07) != std::string::npos){
			buf = ":";
			buf += SERVNAME;
			buf += ERR476 + name + " " + args[i] + MSG476;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue;
		}else if (channel_data.getUserList().size() == 0){
			buf = ":";
			buf += SERVNAME;
			buf += ERR403 + name + " " + args[i] + MSG403;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue; 
		}else if (!channel_data.findUser(name)){
			buf = ":";
			buf += SERVNAME;
			buf += ERR442 + name + " " + args[i] + MSG442;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue;
		}else if (channel_data.getPrivileges(name) != 0){
			buf = ":";
			buf += SERVNAME;
			buf += ERR482 + name + " " + args[i] + MSG482;
			this->_handler.getServer().setFdMessage(fd, buf);
			continue;
		}
		for(size_t j = 0; j < targets.size(); ++j){
			if (!channel_data.findUser(targets[j])){
				buf = ":";
				buf += SERVNAME;
				buf += ERR441 + name + " " + args[i] + MSG441;
				this->_handler.getServer().setFdMessage(fd, buf);
				continue;
			}
			buf = "";
			if (message == "")
				buf.append(":" + name + " KICK : " + channels[i] + " " + targets[j] + " :" + name + "\r\n");
			else
				buf.append(":" + name + "KICK : " + channels[i] + " " + targets[j] + " :" + message + "\r\n");
			this->_handler.getServer().setFdMessage(fd, buf);
			this->_handler.getServer().setFdMessage(this->_handler.getServer().g_db.getUserTable().getUser(targets[j]).fd, buf);	
			target_info = this->_handler.getServer().g_db.getUserTable().getUser(targets[j]);
			this->_handler.getServer().g_db.removeChannel(target_info, channels[i]);
		}
	}
}

std::vector<std::string> Kick::splitByComma(std::string args){
	std::vector<std::string> ret;
	if (args.find(',') == std::string::npos){
		ret.push_back(args);
		return (ret);
	} else{
		std::istringstream stream;
		std::string targets;

		stream.str(args);
		while (std::getline(stream, targets, ','))
			ret.push_back(targets);
		return (ret);
	}
}


void Part::run(int fd, std::vector<std::string> args) {
	std::string name = this->_handler.getServer().getUserName(fd);
	std::string buf;
	for (size_t index = 0; index < args.size(); ++index){
		ChannelData& chn = this->_handler.getServer().g_db.getCorrectChannel(args[index]);
		if (chn.getUserList().size() == 0) {
			buf = ":";
			buf += SERVNAME;
			buf += ERR403 + name + " " + args[index] + MSG403;
			_handler.getServer().setFdMessage(fd, buf);
		} else {
			struct s_user_info user_info = this->_handler.getServer().g_db.getUserTable().getUser(name);
			std::vector<std::string> channel_user = chn.getUserList();
			size_t i(0);
			for (; i < channel_user.size(); ++i) {
				if (channel_user[i] == name)
					break ;
			}
			if (i == channel_user.size()) {
				buf = ":";
				buf += SERVNAME;
				buf += ERR442 + name + " " + args[index] + MSG442;
				_handler.getServer().setFdMessage(fd, buf);
			}
			else {
				chn.removeData(name);
				this->_handler.getServer().g_db.getUserTable().removeChannel(user_info, args[index]);
				buf = "";
				buf += ":" + name + " PART " + ":" + args[index] + "\r\n";
				_handler.getServer().setFdMessage(fd, buf);

				std::vector<std::string> user_list = chn.getUserList();
				int receiver(0);
				for (size_t j = 0; j < user_list.size(); ++j) {
					receiver = this->_handler.getServer().g_db.getUserTable().getUser(user_list[j]).fd;
					this->_handler.getServer().setFdMessage(receiver, buf);
				}
			}
		}
	}
}

void User::run(int fd, std::vector<std::string> args) {
	std::string name = this->_handler.getServer().getUserName(fd);
	if (this->_handler.getServer().getFdFlagsStatus(fd, 2)){
		std::string buf = ":";
		buf += SERVNAME;
		buf += ERR462 + name + MSG462;
		this->_handler.getServer().setFdMessage(fd, buf);
		return ;
	}
	struct s_user_info info =
			this->_handler.getServer().g_db.getUserTable().getUser(name);
	info.name = args[0];
	info.real = args[3];
	this->_handler.getServer().setFdFlagsOn(fd, 2);
	if (this->_handler.getServer().checkGreetingMessage(fd)){
		this->_handler.getServer().setFdMessage(fd, RPL001 + name + MSG001);
		this->_handler.getServer().setFdFlagsOn(fd, 3);
	}
}

void Pass::run(int fd, std::vector<std::string> args) {
	if (this->_handler.getServer().getFdFlagsInitStatus(fd)){
		std::string name = this->_handler.getServer().getUserName(fd);
		std::string buf = ":";
		buf += SERVNAME;
		buf += ERR462 + name + MSG462;
		this->_handler.getServer().setFdMessage(fd, buf);
	}
	else{
		if (this->_handler.getServer().getServerPassword() == args[0])
			this->_handler.getServer().setFdFlagsOn(fd, 0);
	}
}

void Ping::run(int fd, std::vector<std::string> args) {
	std::string buf;
	std::string name = this->_handler.getServer().getUserName(fd);
	if (args.size() < 1) {
		buf = ":";
		buf += SERVNAME;
		buf += ERR461 + name + " PING " + MSG461;
		this->_handler.getServer().setFdMessage(fd, buf);
		return ;
	}
	buf = ":";
	buf += SERVNAME;
	buf += " PONG :" + args[0] + "\r\n";
	this->_handler.getServer().setFdMessage(fd, buf);
}

void Bot::run(int fd, std::vector<std::string> args){
	std::string buf("");
	if (args.size() == 1 && args[0] == "help"){
		buf.append("I am Channel helper\n");
		buf.append("type \"@BOT list\" to get channel list\n");
		buf.append("type \"@BOT list <channel name>\" to get User list in Channel\r\n");
		this->_handler.getServer().setFdMessage(fd, buf);
		return ;
	}else if (args.size() == 1 && args[0] == "list"){
		std::vector<std::string> list = this->_handler.getServer().g_db.getChannelList();
		buf.append("===Current Channel List===\n");
		for (size_t i = 0; i < list.size(); ++i){
			if (this->_handler.getServer().g_db.getCorrectChannel(list[i]).getUserList().size() != 0)
				buf.append("-" + list[i] + "\n");
		}
		buf.append("===end of list===\r\n");
		this->_handler.getServer().setFdMessage(fd, buf);
	}else if (args.size() == 2 && args[0] == "list"){
		if ((args[1][0] != '#' && args[1][0] != '&') || args[1].find(0x07) != std::string::npos) {
			buf = ":";
			buf += SERVNAME;
			buf += ERR476 + this->_handler.getServer().getUserName(fd) + " " + args[1] + MSG476;
			this->_handler.getServer().setFdMessage(fd, buf);
			return;
		} else if (this->_handler.getServer().g_db.getCorrectChannel(args[1]).getUserList().size() == 0){
			buf = ":";
			buf += SERVNAME;
			buf += ERR403 + this->_handler.getServer().getUserName(fd) + " " + args[1] + MSG403;
			_handler.getServer().setFdMessage(fd, buf);
			return;
		}
		buf.append("Active User List in [" + args[1] + "]\n");
		std::vector<std::string> temp = this->_handler.getServer().g_db.getCorrectChannel(args[1]).getUserList();
		for(size_t i = 0; i < temp.size(); ++i)
			buf.append("-" + temp[i] + "\n");
		buf.append("===end of list===\r\n");
		this->_handler.getServer().setFdMessage(fd, buf);
	}else {
		buf.append("Error :type @BOT help\r\n");
		this->_handler.getServer().setFdMessage(fd, buf);
	}
}
