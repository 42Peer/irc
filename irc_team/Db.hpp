#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

// using namespace std;

struct s_user_info {
    int fd;
	int privileges;
    std::string nick;
    std::string name;
	std::vector<std::string> channel_list;
};


class ChannelData
{
public:
    typedef std::map<std::string, int>::iterator iter;

	int	grantUser() const {
		return (_tables.size() >= 1);
	}

    bool isExist(const std::string& nick) const {
        return (_tables.find(nick) != _tables.end());
    }

	int getPrivileges(const std::string& nick) {
		if (isExist(nick)) {
			return (_tables[nick]);
		}
		return (-1);
	}

    bool addData(struct s_user_info& user) {
        if (isExist(user.nick)) {
            return false;
        }
		int privileges = grantUser();
        _tables.insert(std::pair<std::string, int>(user.nick, privileges));
        return true;
    }

	bool isEmpty() const {
		return (!_tables.empty());
	}

    void removeData(const std::string& nick) {
		iter it = _tables.find(nick);
		if (it != _tables.end())
        	_tables.erase(nick);
    }

    void printTables() {
        for (iter it = _tables.begin(); it != _tables.end(); ++it) {
            if (it->second == 0)
                std::cout << "방장";
            else
                std::cout << "User";
            std::cout << "[user nick : " << it->first<< "]\n";
        }
    }

private:
    std::map<std::string, int> _tables;
};

class UserData
{
public:
    typedef  std::map<std::string, struct s_user_info>::iterator iter;
    bool isExist(const std::string& nick) const {
		return (_tables.find(nick) != _tables.end());
    }

    bool addUser(struct s_user_info& user) {
        if (isExist(user.nick))
            return (false);
        struct s_user_info info;

		info.privileges = user.privileges;
        info.fd = user.fd;
        info.name = user.name;
        info.nick = user.nick;
		info.channel_list = user.channel_list;
        _tables.insert(std::pair<std::string, struct s_user_info>(user.nick, info));
        return (true);
    }

    int userFd(const std::string& nick) {
        iter it = _tables.find(nick);
        if (it == _tables.end())
            return (-1);
        return it->second.fd;
    }

    void printTables() {
		std::vector<std::string>::iterator vi;
        for (iter it = _tables.begin(); it != _tables.end(); ++it) {
            if (it->second.privileges == 0)
                std::cout << "방장";
            else if (it->second.privileges == -1)
                std::cout << "not entered room" << std::endl;
            else
                std::cout << "User";
            std::cout << "[user name : " << it->second.name << "]";
            std::cout << "[user nick : " << it->second.nick << "]\n";
			std::cout <<"===== Channel list =====\n";
			for (vi = it->second.channel_list.begin(); vi != it->second.channel_list.end(); ++vi)
			{
				std::cout << *vi << " ";
			}
			std::cout << "\n";
        }
    }

	void updateUser(struct s_user_info org, struct s_user_info usr) {
		if (_tables.find(org.nick) != _tables.end()) {
			_tables.erase(org.nick);
		}
		_tables.insert(std::pair<std::string, struct s_user_info>(usr.nick, usr));
	}

    void removeUser(std::string key) {
        _tables.erase(key);
    }

	void setPrivileges(std::string id, int privileges) {
		_tables[id].privileges = privileges;
	}

	void addChannel(struct s_user_info& usr, const std::string& channel_name) {
		for (int i = 0; i < _tables[usr.nick].channel_list.size(); ++i) {
			if (_tables[usr.nick].channel_list[i] == channel_name)
				return ;
		}
		_tables[usr.nick].channel_list.push_back(channel_name);
		_tables[usr.nick].privileges = 1;
	}
	
	void removeChannel(struct s_user_info& usr, const std::string& channel_name) {
		std::vector<std::string>::iterator viter = _tables[usr.nick].channel_list.begin();

		for (; viter != _tables[usr.nick].channel_list.end(); ++viter) {
			if (*viter == channel_name) {
				_tables[usr.nick].channel_list.erase(viter);
				if (_tables[usr.nick].channel_list.empty())
					_tables[usr.nick].privileges = -1;
				return ;
			}
		}
	}

private:
    std::map<std::string, struct s_user_info> _tables;
};

class Db
{
public:
    typedef std::map<std::string, ChannelData>::iterator iter;

    ChannelData& getCorrectChannel(const std::string& channelName) {
        if (channel_tables.find(channelName) == channel_tables.end()) {
            this->addChannel(channelName);
        }
        return (channel_tables[channelName]);
    }

    bool addChannel(const std::string& cname) {
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
        user_table.addUser(user);
    }

    void printChannelTables() {
        for (iter it = channel_tables.begin(); it != channel_tables.end(); ++it) {
            std::cout << "[ channel name :    " << it->first << "   ]\n";
            it->second.printTables();
        }
    }

	void printUserTables() {
		user_table.printTables();
	}

    void removeChannel(struct s_user_info &user, const std::string& key) {
		/* iterator erase*/
        iter it;
        if ((it = channel_tables.find(key)) != channel_tables.end()) {
			channel_tables[key].removeData(user.nick);
			if (channel_tables[key].isEmpty()) {
                channel_tables.erase(it);
            }
        }
		user_table.removeChannel(user, key);
    }

    void removeUser(struct s_user_info user) {
        iter it = channel_tables.begin();
        while (it != channel_tables.end())
        {
            iter tmp = it;
			channel_tables[it->first].removeData(user.nick);
            if (!channel_tables[it->first].isEmpty()) {
                ++it;
                channel_tables.erase(tmp);
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
			int privileges = channel_tables[it->first].getPrivileges(org.nick);
			if (privileges != -1) {
				channel_tables[it->first].removeData(org.nick);
				channel_tables[it->first].addData(new_user);
			}
           	++it;
        }
		user_table.updateUser(org, new_user);
	}

	void addChannelUser(struct s_user_info& usr, const std::string& channel_name) {
		if (!user_table.isExist(usr.nick))
			user_table.addUser(usr);
		user_table.addChannel(usr, channel_name);
		ChannelData chn = getCorrectChannel(channel_name);
		chn.addData(usr);
	}

private:
    std::map<std::string, ChannelData> channel_tables;
    UserData user_table;
};


#endif /* __DB_HPP_ */
