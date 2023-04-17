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
		buf.append(":" + this->_handler.getServer().getUserName(fd) + MSGNOTICE + args.front() + " :" + args.back() + "\r\n");
		int receiver = this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd;
		this->_handler.getServer().setFdMessage(receiver, buf);
	}
}


void Join::run(int fd, std::vector<std::string> args) {
	std::string nick_name(this->_handler.getServer().getUserName(fd));
	std::string buf("");
	for (int i = 0; i < args.size(); ++i) {
		if (args[i][0] != '#') {
			this->_handler.getServer().setFdMessage(fd, ERR476);
			return;
		} else {
			struct s_user_info& curr_user = this->_handler.getServer().g_db.getUserTable().getUser(nick_name);
			this->_handler.getServer().g_db.addChannelUser(curr_user, args[i]);
			ChannelData chn = this->_handler.getServer().g_db.getCorrectChannel(args[i]);

			buf = ":" + nick_name + MSGJOIN + args[i] + "\r\n";

			std::vector<std::string> user_list = chn.getUserList();
			int receiver(0);
			for (int j = 0; j < user_list.size(); ++j) {
				receiver = this->_handler.getServer().g_db.getUserTable().getUser(user_list[j]).fd;
				this->_handler.getServer().setFdMessage(receiver, buf);
			}
			buf.clear();
		}
	}
}


void Nick::run(int fd, std::vector<std::string> args) {
	std::string buf("");
	std::string new_nick = args[0];
	if (new_nick[0] == '#' || new_nick.find(',') != std::string::npos || new_nick.find(' ') != std::string::npos) {
		this->_handler.getServer().setFdMessage(fd, ERR432);
		return;
	}
	else if (this->_handler.getServer().g_db.getUserTable().isExist(new_nick)) {
		this->_handler.getServer().setFdMessage(fd, ERR433);
		return;
	} else {
		struct s_user_info new_user_info;
		new_user_info.nick = new_nick;
		if (this->_handler.getFdflags().find(fd) != this->_handler.getFdflags().end() && \
			this->_handler.getFdflags()[fd] == 1) {
			new_user_info.fd = fd;
			this->_handler.setFdFlags(fd, 2);
			this->_handler.getServer().g_db.getUserTable().addUser(new_user_info);
		}
		else{
			std::string old_name = this->_handler.getServer().getUserName(fd);
			s_user_info old_user_info = this->_handler.getServer().g_db.getUserTable().getUser(old_name);
			new_user_info.name = old_user_info.name;
			new_user_info.fd = old_user_info.fd;

			this->_handler.getServer().g_db.updateUser(old_user_info, new_user_info);
			buf.append(":" + this->_handler.getServer().getUserName(fd) + MSGNICK + new_nick + "\r\n");

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
		this->_handler.getServer().setMapData(fd, new_nick);
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
									user_lists[j] + "\r\n");
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
												 cur_user.nick + "\r\n");
	}
}


void Kick::run(int fd, std::vector<std::string> args) {
	std::string msg("");
	std::string channel_name = args[0];
	std::string target_name = args[1];
	std::string user_name = this->_handler.getServer().getUserName(fd);
	ChannelData tmp_channel = _handler.getServer().g_db.getCorrectChannel(channel_name);
	

	if (channel_name[0] != '#') {
		_handler.getServer().setFdMessage(fd, ERR476); // Channel Mask err
		return;
	} else if (tmp_channel.getUserList().size() == 0){
		this->_handler.getServer().setFdMessage(fd, ERR403); // Empty Channel
		return ;
	} else if (!tmp_channel.findUser(user_name) || !tmp_channel.findUser(target_name)){
		this->_handler.getServer().setFdMessage(fd, ERR442); // Not in Channel
		return ;
	} else if (tmp_channel.getPrivileges(user_name) != 0){
		this->_handler.getServer().setFdMessage(fd, ERR482);
		return ;
	} else if (args.size() == 3){
		msg = args.back();
	}
	std::string buf("");
	if (msg == "")
		buf.append("KICK : " + target_name + " from " + channel_name + "\r\n");
	else
		buf.append("KICK : " + target_name + " from " + channel_name + " using " + msg + " as the reason.\r\n");
	
	this->_handler.getServer().setFdMessage(fd, buf);
	this->_handler.getServer().setFdMessage(this->_handler.getServer().g_db.getUserTable().getUser(target_name).fd, buf);
	struct s_user_info target_info = this->_handler.getServer().g_db.getUserTable().getUser(target_name);
	this->_handler.getServer().g_db.removeChannel(target_info, channel_name);
}

void Part::run(int fd, std::vector<std::string> args) {
	std::string name = this->_handler.getServer().getUserName(fd);
	std::vector<std::string>::iterator it = args.begin();
	for (size_t index = 0; index < args.size(); ++index){
		ChannelData& chn = this->_handler.getServer().g_db.getCorrectChannel(args[index]);
		if (chn.getUserList().size() == 0) {
				_handler.getServer().setFdMessage(fd, ERR403);
		} else {
			struct s_user_info user_info = this->_handler.getServer().g_db.getUserTable().getUser(name);
			std::vector<std::string> channel_user = chn.getUserList();
			size_t i(0);
			for (; i < channel_user.size(); ++i) {
				if (channel_user[i] == name)
					break ;
			}
			if (i == channel_user.size())
				_handler.getServer().setFdMessage(fd, ERR442);
			else {
				chn.removeData(name);
				this->_handler.getServer().g_db.getUserTable().removeChannel(user_info, args[index]);
			}
		}
	}
}

void User::run(int fd, std::vector<std::string> args) {
    std::cout << "user income " << std::endl;
    for (int i = 0; i < args.size(); ++i) {
        std::cout << args[i] << " ";
    }
    std::cout << "\n";
	if (this->_handler.getFdflags().find(fd) == this->_handler.getFdflags().end()){
		this->_handler.getServer().setFdMessage(fd, ERR462);
		return ;
	}
	std::string name = this->_handler.getServer().getUserName(fd);
	struct s_user_info info =
			this->_handler.getServer().g_db.getUserTable().getUser(name);
	info.name = args[0];
	info.real = args[3];
	this->_handler.getFdflags().erase(fd);
	this->_handler.getServer().setFdMessage(fd, CODEGREET + name +  MSGGREETING);
}

void Pass::run(int fd, std::vector<std::string> args) {
    std::cout << "PASS\n";
	if (this->_handler.getFdflags().find(fd) == this->_handler.getFdflags().end()){
		this->_handler.getServer().setFdMessage(fd, ERR462);
	}
	else{
		if (this->_handler.getServer().getServerPassword() == args[0])
			this->_handler.setFdFlags(fd, 1);
	}
}