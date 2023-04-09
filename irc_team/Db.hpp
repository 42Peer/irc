#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace std;

struct s_user_info {
    int fd;
    int privileges;
    std::string nick;
    std::string name;
};


class ChannelData
{
public:
    typedef std::map<std::string, s_user_info>::iterator iter;

    bool isExist(std::string nick) {
        if (_tables.size() > 0 && _tables.find(nick) != _tables.end())
            return (true);
        return (false);
    }

	int getPrivileges(std::string nick) {
		if (isExist(nick)) {
			return (_tables[nick].privileges);
		}
		return (-1);
	}

    bool addData(int privilege, s_user_info& user) {
        if (isExist(user.nick)) {
            std::cerr << "nick name 중복됨." << std::endl;
            return false;
        }
        struct s_user_info info;

        info.privileges = privilege;
        info.fd = user.fd;
        info.name = user.name;
        info.nick = user.nick;
        _tables.insert(std::pair<std::string, struct s_user_info>(user.nick, info));
        return true;
    }

    bool removeData(std::string nick) {
        _tables.erase(nick);
        if (_tables.size() >= 1)
            return false;
        return true;
    }

    void printTables() {
        for (iter it = _tables.begin(); it != _tables.end(); ++it) {
            if (it->second.privileges == 0)
                std::cout << "방장";
            else
                std::cout << "User";
            std::cout << "[user name : " << it->second.name << "]";
            std::cout << "[user nick : " << it->second.nick << "]\n";
        }
    }

private:
    std::map<std::string, struct s_user_info> _tables;
};

class UserData
{
public:
    typedef  std::map<std::string, struct s_user_info>::iterator iter;
    bool isExist(std::string nick) {
        if (_tables.size() > 0 && _tables.find(nick) != _tables.end())
            return (true);
        return (false);
    }

    bool addUser(int privileges, s_user_info& user) {
        if (isExist(user.nick))
            return (false);
        struct s_user_info info;

        info.privileges = privileges;
        info.fd = user.fd;
        info.name = user.name;
        info.nick = user.nick;
        _tables.insert(std::pair<std::string, struct s_user_info>(user.nick, info));
        return (true);
    }

    int userFd(std::string nick) {
        iter it = _tables.find(nick);
        if (it == _tables.end())
            return (-1);
        return it->second.fd;
    }

    void printTables() {
        for (iter it = _tables.begin(); it != _tables.end(); ++it) {
            if (it->second.privileges == 0)
                std::cout << "방장";
            else if (it->second.privileges == -1)
                std::cout << "not entered room" << std::endl;
            else
                std::cout << "User";
            std::cout << "[user name : " << it->second.name << "]";
            std::cout << "[user nick : " << it->second.nick << "]\n";
        }
    }

	void updateUser(struct s_user_info org, struct s_user_info _new) {
		if (_tables.find(org.nick) != _tables.end()) {
			_tables.erase(org.nick);
		}
		_tables.insert(std::pair<std::string, struct s_user_info>(_new.nick, _new));
	}

    void removeUser(std::string key) {
        _tables.erase(key);
    }
private:
    std::map<std::string, struct s_user_info> _tables;
};

class Db
{
public:
    typedef std::map<std::string, ChannelData>::iterator iter;

    ChannelData& getCorrectChannel(std::string channelName) {
        if (channel_tables.find(channelName) == channel_tables.end()) {
            this->addChannel(channelName);
        }
        return (channel_tables[channelName]);
    }

    bool addChannel(std::string cname) {
        if (channel_tables.find(cname) == channel_tables.end()) {
            ChannelData chn;
            channel_tables.insert(std::pair<std::string, ChannelData>(cname, chn));
            return (true);
        }
        return (false);
    }

    UserData&   getUserTable() {
        return (user_table);
    }

    void addUser(s_user_info& user) {
        if (user_table.isExist(user.nick))
            return ;
        user_table.addUser(-1,user);
    }

    void printChannelTables() {
        for (iter it = channel_tables.begin(); it != channel_tables.end(); ++it) {
            std::cout << "[ channel name :    " << it->first << "   ]\n";
            it->second.printTables();
        }
    }

    void removeChannel(std::string key, struct s_user_info &user) {
        if (channel_tables.find(key) != channel_tables.end()) {
            if (channel_tables[key].removeData(user.nick)) {
                channel_tables.erase(key);
            }
        }
    }

    void removeUser(struct s_user_info user) {
        iter it = channel_tables.begin();
        while (it != channel_tables.end())
        {
            iter tmp = it;
            if (channel_tables[it->first].removeData(user.nick)) {
                ++it;
                channel_tables.erase(tmp->first);
                if (it == channel_tables.end())
                    break ;
            } else {
                ++it;
            }
        }
        user_table.removeUser(user.nick);
    }

	void updateUser(struct s_user_info& org, struct s_user_info& new_user) {
		iter it = channel_tables.begin();
		while (it != channel_tables.end())
        {
            iter tmp = it;
			int privileges = channel_tables[it->first].getPrivileges(org.nick);
			if (privileges != -1) {
				channel_tables[it->first].removeData(org.nick);
				channel_tables[it->first].addData(privileges, new_user);
			}
           	++it;
        }
		user_table.updateUser(org, new_user);
	}

private:
    std::map<std::string, ChannelData> channel_tables;
    UserData user_table;
};


#endif /* __DB_HPP_ */
