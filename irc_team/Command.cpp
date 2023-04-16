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

std::vector<std::string> getChannelUser(Db &db, std::string name) {
	struct s_user_info user_data;
	user_data = db.getUserTable().getUser(name);
	std::string channel_name = db.getUserTable().getChannelList(user_data);
	return db.getCorrectChannel(channel_name).getUserList();
}

void Notice::run(int fd, std::vector<std::string> args) {
	std::vector<std::string>::iterator it = args.begin();
	std::string buf("");
	if (args.front()[0] == '#') {
		buf.append(ERR404);
		this->_handler.getServer().setFdMessage(fd, buf);
	}
	else if (this->_handler.getServer().g_db.getUserTable().isExist(args.front()) == false) {
		buf.append(ERR401);
		this->_handler.getServer().setFdMessage(fd, buf);
	}
	else {
		buf.append(":" + this->_handler.getServer().getUserName(fd) + MSGNOTICE + args.front() + " :" + args.back() + "\n");
		int receiver = this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd;
		this->_handler.getServer().setFdMessage(receiver, buf);
	}
}


void Join::run(int fd, std::vector<std::string> args) {
	std::vector<std::string>::iterator it_channel_name = args.begin();
	std::vector<std::string>::iterator eit = args.end();
	std::string nick_name = this->_handler.getServer().getUserName(fd);
	std::string buf("");
	for (int i = 0; it_channel_name != eit; ++it_channel_name, ++i) {
		if (args[i][0] != '#') {
			this->_handler.getServer().setFdMessage(fd, ERR476);
			return;
		} else {
			this->_handler.getServer().g_db.addChannelUser(this->_handler.getServer().g_db.getUserTable().getUser(nick_name), *it_channel_name);
			buf.append(":" + nick_name + MSGJOIN + *it_channel_name + "\n");
			// 채널에 속한 유저들에게 똑같은 메세지를 send() 해줘야함
			std::vector<std::string> userList = this->_handler.getServer().g_db.getCorrectChannel(*it_channel_name).getUserList();
			std::vector<std::string>::iterator it_users = userList.begin();
			std::vector<std::string>::iterator eit_users = userList.end();
			int receiver(0);
			for (; it_users != eit_users; ++it_users) {
				receiver = this->_handler.getServer().g_db.getUserTable().getUser(*it_users).fd;
				this->_handler.getServer().setFdMessage(receiver, buf);
			}
		}
	}
}


void Nick::run(int fd, std::vector<std::string> args) {
	std::string buf("");
	std::string new_nick = args[0];
	if (new_nick[0] == '#') {
		this->_handler.getServer().setFdMessage(fd, ERR432);
		return;
	}
	if (this->_handler.getServer().g_db.getUserTable().isExist(new_nick)) {
		this->_handler.getServer().setFdMessage(fd, ERR433);
		return;
	} else {
		std::string old_name = this->_handler.getServer().getUserName(fd);
		s_user_info old_user_info = this->_handler.getServer().g_db.getUserTable().getUser(old_name);
		s_user_info new_user_info;
		new_user_info.nick = new_nick;
		new_user_info.name = old_user_info.name;
		new_user_info.fd = old_user_info.fd;
		new_user_info.channel_list = old_user_info.channel_list;

		this->_handler.getServer().setMapData(fd, args.front());
		this->_handler.getServer().g_db.updateUser(old_user_info, new_user_info);

		buf.append(":" + this->_handler.getServer().getUserName(fd) + MSGNICK + new_nick + "\n");

		if (this->_handler.getFdflags().find(fd) != this->_handler.getFdflags().end()) {
		this->_handler.setFdFlags(fd, 2);
		this->_handler.getServer().g_db.getUserTable().getUser(new_nick).fd = fd;
	}

		// 같은 채널 사람들에게 보여주기
		std::string current_channel = this->_handler.getServer().g_db.getUserTable().getChannelList(new_user_info);
		std::vector<std::string> users_in_current_channel = this->_handler.getServer().g_db.getCorrectChannel(current_channel).getUserList();
		std::vector<std::string>::iterator it_users = users_in_current_channel.begin();
		std::vector<std::string>::iterator eit_users = users_in_current_channel.end();
		int receiver(0);
		for (; it_users < eit_users; ++it_users) {
			receiver = this->_handler.getServer().g_db.getUserTable().getUser(*it_users).fd;
			this->_handler.getServer().setFdMessage(receiver, buf);
		}
	}
}


void Quit::run(int fd, std::vector<std::string> args) {
	(void)args;
	std::string usr_name = this->_handler.getServer().getUserName(fd);
	struct s_user_info usr_name_info =
			this->_handler.getServer().g_db.getUserTable().getUser(usr_name);
	this->_handler.getServer().g_db.removeUser(usr_name_info);
	this->_handler.getServer().removeMapData(fd);
	close(fd);
}


std::vector<bool> duplicated_args(std::vector<std::string>& args) {
	std::map<std::string, int> check;
	std::vector<bool> ret_arg;
	ret_arg.resize(args.size() - 1);
	int cnt = 0;
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
	std::vector<bool> res_args = duplicated_args(args);
	std::vector<std::string> users;
	UserData user = this->_handler.getServer().g_db.getUserTable();
	std::string msg;
	msg = ":" + this->_handler.getServer().getUserName(fd) + " PRIVMSG ";
	for (size_t i = 0; i < args.size() - 1; ++i) {
		if (!user.isExist(args[i])) {
			if (args[i][0] == '#') {
				ChannelData chn =
						this->_handler.getServer().g_db.getCorrectChannel(args[i]);
				if (!chn.findUser(this->_handler.getServer().getUserName(fd))){
					this->_handler.getServer().setFdMessage(fd, ERR442);
					continue;
				}
				if (res_args[i] == true) {
					this->_handler.getServer().setFdMessage(fd, ERR407);
					continue;
				}
				std::vector<std::string> user_lists = chn.getUserList();
				for (size_t j = 0; j < user_lists.size(); ++j) {
					this->_handler.getServer().setFdMessage(
							user.getUser(user_lists[j]).fd,
							msg + user_lists[j] + " :" + args.back() + ";\n\t" +
									" Message from " +
									this->_handler.getServer().getUserName(fd) + " to " +
									user_lists[j] + "\n");
				}
				continue;
			}
			this->_handler.getServer().setFdMessage(fd, ERR401);
			continue;
		}
		if (res_args[i] == true) {
			this->_handler.getServer().setFdMessage(fd, ERR407);
			continue;
		}
		struct s_user_info cur_user = user.getUser(args[i]);
		this->_handler.getServer().setFdMessage(
				cur_user.fd, msg + cur_user.nick + " :" + args.back() + ";" +
												 " Message from " + this->_handler.getServer().getUserName(fd) + " to " +
												 cur_user.nick + "\n");
	}
}


void Kick::run(int fd, std::vector<std::string> args) {
	std::string tmp_msg = "";

	if (args[0][0] != '#') {
		_handler.getServer().setFdMessage(fd, ERR401); // ERR 확인!!
		return;
	} else {
		bool check = _handler.getServer().g_db.addChannel(args[0]);
		_handler.getServer().g_db.channelClear(args[0]);
		if (check) { // channel 존재여부 확인
			_handler.getServer().setFdMessage(fd, ERR442);
			return;
		}
	}

	ChannelData tmp_channel =
			_handler.getServer().g_db.getCorrectChannel(args[1]);
	if (!_handler.getServer().g_db.isExist(args[1])) {
		_handler.getServer().setFdMessage(fd, ERR401);
		return;
	} else {
		if (!tmp_channel.findUser(args[1])) {
			_handler.getServer().setFdMessage(fd, ERR482);
			return;
		}
	}

	std::string tmp_name = _handler.getServer().getUserName(fd);
	if (!tmp_channel.findUser(tmp_name)) {
		_handler.getServer().setFdMessage(fd, ERR442);
		return;
	}

	if (tmp_channel.getPrivileges(tmp_name) > 0) {
		_handler.getServer().setFdMessage(fd, ERR482);
	} else {
		tmp_channel.removeData(args[1]);
		tmp_msg = "KICK " + args[0] + " " + args[1];
		if (args.size() == 3) {
			tmp_msg += ":" + args[2] + "\n";
			_handler.getServer().setFdMessage(fd, tmp_msg);
		} else {
			tmp_msg += "\n";
			_handler.getServer().setFdMessage(fd, tmp_msg);
		}
	}
}


void Part::run(int fd, std::vector<std::string> args) {

	std::string name = this->_handler.getServer().getUserName(fd);

	if (this->_handler.getServer()
					.g_db.getUserTable()
					.getUser(name)
					.channel_list.size() == 0) {
		_handler.getServer().setFdMessage(fd, ERR442);
		return;
	} else if (!args.size()) {
		std::string current_channel = this->_handler.getServer().g_db.getUserTable().getUser(name).channel_list.back();
		this->_handler.getServer().g_db.getUserTable().removeChannel(this->_handler.getServer().g_db.getUserTable().getUser(name),current_channel);
	} else {
		std::vector<std::string>::iterator it = args.begin();
		while (it != args.end()) {
				ChannelData& chn = this->_handler.getServer().g_db.getCorrectChannel(*it);
				if (chn.getUserList()[0] == "") {
						this->_handler.getServer().g_db.channelClear(*it);
						_handler.getServer().setFdMessage(fd, ERR403);
						_handler.getServer().setFdMessage(fd, *it);
						_handler.getServer().g_db.channelClear(*it);
				} else {
						std::vector<std::string> channel_user = chn.getUserList();
						size_t i;
						for (i = 0; i < channel_user.size(); ++i) {
								if (channel_user[i] == name) {
										break ;
								}
						}
						if (i == channel_user.size()) {
								_handler.getServer().setFdMessage(fd, ERR442);
								_handler.getServer().setFdMessage(fd, *it);
						} else {
								chn.removeData(name);
								this->_handler.getServer().g_db.getUserTable().removeChannel(this->_handler.getServer().g_db.getUserTable().getUser(name), *it);
						}
				}
			++it;
		}
	}
}


void User::run(int fd, std::vector<std::string> args) {
	if (this->_handler.getFdflags().find(fd) == this->_handler.getFdflags().end()){
		this->_handler.getServer().setFdMessage(fd, ERR462);
	}
	std::string name = this->_handler.getServer().getUserName(fd);
	struct s_user_info info =
			this->_handler.getServer().g_db.getUserTable().getUser(name);
	info.name = args[0];
	info.real = args[3];
	this->_handler.getFdflags().erase(fd);
	this->_handler.getServer().setFdMessage(fd, MSGGREETING);
}

void Pass::run(int fd, std::vector<std::string> args) {
	if (this->_handler.getFdflags().find(fd) == this->_handler.getFdflags().end()){
		this->_handler.getServer().setFdMessage(fd, ERR462);
	}
	else{
		if (this->_handler.getServer().getServerPassword() == args[0])
			this->_handler.setFdFlags(fd, 1);
	}
}