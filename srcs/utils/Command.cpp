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

// std::vector<std::string> getChannelUser(Db &db, std::string name) {
// 	struct s_user_info user_data;
// 	user_data = db.getUserTable().getUser(name);
// 	std::string channel_name = db.getUserTable().getChannelList(user_data);
// 	return db.getCorrectChannel(channel_name).getUserList();
// }

void Notice::run(int fd, std::vector<std::string> args) {
	// std::vector<std::string>::iterator it = args.begin();
	std::string buf("");
	std::string my_name = this->_handler.getServer().getUserName(fd);
	if (args.front()[0] == '#') {
//		:irc.local 404 jujeon nickname :No such nick
		buf.append(":");
		buf.append(SERVNAME);
		buf.append(ERR404 + my_name + " " + args[0] + MSG404);
		this->_handler.getServer().setFdMessage(fd, buf);
	}
	else if (this->_handler.getServer().g_db.getUserTable().isExist(args.front()) == false) {
//		:irc.local 401 jujeon nickname :No such nick
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
		// 같은 채널에 조인하려고 했을때
		if (this->_handler.getServer().g_db.getUserTable().getUser(nick_name).channel_list.size() > 0
				&& this->_handler.getServer().g_db.getUserTable().getUser(nick_name).channel_list.back() == args[i]) {
			continue;
		}
		else {
			struct s_user_info& curr_user = this->_handler.getServer().g_db.getUserTable().getUser(nick_name);
			this->_handler.getServer().g_db.addChannelUser(curr_user, args[i]);
			ChannelData chn = this->_handler.getServer().g_db.getCorrectChannel(args[i]);

			// find the king in the channel
			std::vector<std::string> user_list = chn.getUserList();
			std::string king("");
			for (size_t h = 0; h < user_list.size(); ++h) {
				if (this->_handler.getServer().g_db.getCorrectChannel(args[i]).getPrivileges(user_list[h]) == 0) {
					king = user_list[h];
					break ;
				}
			}

			buf = ":" + nick_name + " JOIN " + args[i] + "\n"
					+ ":" + SERVNAME + "353 " + nick_name + " = " + args[i] + " :@" + king + "\n"
					+ ":" + SERVNAME + "366 " + nick_name + " " + args[i] + " :End of /NAMES list.\r\n";

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
	{
			std::cout << "new client tmp nick is *" << "\n";
		current_nick = "*";
	}

	if (!isValidName(new_nick)) {
    // :irc.local 432 jujeon *juje :Erroneous Nickname
		buf.append(":" + SERVNAME + " 432 " + current_nick + " " + new_nick + MSG432);
		this->_handler.getServer().setFdMessage(fd, buf);
		return;
	}
	if (this->_handler.getServer().g_db.getUserTable().isExist(new_nick)) {
    // :irc.local 433 jujeon root :Nickname is already in use.
		buf.append(":" + SERVNAME + " 433 " + current_nick + " " + new_nick + MSG433);
		this->_handler.getServer().setFdMessage(fd, buf);
		return;
	} 

	s_user_info old_user_info = this->_handler.getServer().g_db.getUserTable().getUser(current_nick);
	// Pass 통과 했으면
	if (this->_handler.getServer().getFdFlagsStatus(fd, 0))
	{
		// 가입 전이면 새 client 구조체 할당
		if (!this->_handler.getServer().getFdFlagsStatus(fd, 1))
		{
			std::cout << "new client info created" << "\n";
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

		buf.append(":" + this->_handler.getServer().getUserName(fd) + " NICK " + new_nick + "\r\n");
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
	struct s_user_info usr_name_info =
			this->_handler.getServer().g_db.getUserTable().getUser(usr_name);

//	QUIT jujeon QUIT :Quit: hi
//	채널 안의 멤버들에게도 메세지 보여주기
//	일단보류
//	ChannelData& chn = this->_handler.getServer().g_db.getCorrectChannel(args);
//	std::vector<std::string> user_list = chn.getUserList();
//	int receiver(0);
//	for (size_t j = 0; j < user_list.size(); ++j) {
//		receiver = this->_handler.getServer().g_db.getUserTable().getUser(user_list[j]).fd;
//		this->_handler.getServer().setFdMessage(receiver, buf);
//	}

//  이대로 하게 되면 클라이언트가 메세지를 출력받지 못하고 종료된다. 재접속시 QUIT 메세지가 출력되고 접속되는 문제발생.근본적인 구조 변경 필요.
//	std::string buf("");
//	if (!args.empty())
//		buf = "ERROR :Closing link: [QUIT:" + args[0] + "]\r\n";
//	else
//		buf = "ERROR :Closing link: [QUIT: Client exited]\r\n";
//	this->_handler.getServer().setFdMessage(fd, buf);
	this->_handler.getServer().g_db.removeUser(usr_name_info);
	this->_handler.getServer().removeMapData(fd);
	this->_handler.getServer().setFdFlagsOn(fd, 4);
}

std::vector<bool> Privmsg::checkduplicatedArgs(std::vector<std::string>& args) {
	std::map<std::string, int> check;
	std::vector<bool> ret_arg;
	ret_arg.resize(args.size() - 1);
	// int cnt = 0;
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
//					:irc.local 404 jujeon #123 :You cannot send external messages to this channel whilst the +n (noextmsg) mode is set.
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
					this->_handler.getServer().setFdMessage(
							user.getUser(user_lists[j]).fd, msg + args[i] + " :" + args.back() + "\r\n");
				}
				continue;
			}
//			:irc.local 401 jujeon j :No such nick
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
			this->_handler.getServer().setFdMessage(fd, buf); //channel mask
			continue;
		}else if (channel_data.getUserList().size() == 0){
			buf = ":";
			buf += SERVNAME;
			buf += ERR403 + name + " " + args[i] + MSG403;
			this->_handler.getServer().setFdMessage(fd, buf); // no such channel
			continue; 
		}else if (!channel_data.findUser(name)){
			buf = ":";
			buf += SERVNAME;
			buf += ERR442 + name + " " + args[i] + MSG442;
			this->_handler.getServer().setFdMessage(fd, buf); // not in channel
			continue;
		}else if (channel_data.getPrivileges(name) != 0){
//			:irc.local 482 root #123 :You must be a channel operator
			buf = ":";
			buf += SERVNAME;
			buf += ERR482 + name + " " + args[i] + MSG482;
			this->_handler.getServer().setFdMessage(fd, buf); //privileges
			continue;
		} // init error check
		for(size_t j = 0; j < targets.size(); ++j){
			if (!channel_data.findUser(targets[j])){
				buf = ":";
				buf += SERVNAME;
				buf += ERR441 + name + " " + args[i] + MSG441;
				this->_handler.getServer().setFdMessage(fd, buf); // target is not in channel
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
//			:irc.local 403 jujeon #hi :No such channel
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
//				:irc.local 442 jujeon #11 :You're not on that channel
				buf = ":";
				buf += SERVNAME;
				buf += ERR442 + name + " " + args[index] + MSG442;
				_handler.getServer().setFdMessage(fd, buf);
			}
			else {
//				:root!root@127.0.0.1 PART :#123
				chn.removeData(name);
				this->_handler.getServer().g_db.getUserTable().removeChannel(user_info, args[index]);
				buf = "";
				buf += ":" + name + " PART " + ":" + args[index] + "\r\n";
				_handler.getServer().setFdMessage(fd, buf);

				// 채널 안의 멤버들에게도 메세지 보여주기
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
//		:irc.local 462 jujeon :You may not reregister
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
	if (this->_handler.getServer().getFdFlagsStatus(fd, 0)){
//		:irc.local 462 jujeon :You may not reregister
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
		//not enough parameters
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