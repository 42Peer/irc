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

	std::string getChannelList(struct s_user_info& usr) {
		if (isExist(usr.nick))
		{
			iter it = _tables.find(usr.nick); 
			if (it->second.channel_list.size() > 0)
				return it->second.channel_list[it->second.channel_list.size() - 1];
		}
		return static_cast<std::string>("");
	}

	s_user_info& getUser(const std::string& id) {
		return (_tables[id]);
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

	bool isExist(const std::string& id) {
		return (user_table.isExist(id));
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
		fd_tables.insert(std::pair<int, std::string>(user.fd, user.nick));	
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

	template<typename T>
    void removeChannel(T param, const std::string& key) {
		/* iterator erase*/
		struct s_user_info user;
		if (typeid(T) != typeid(struct s_user_info))
		{
			std::string id = convertFdToId(param);
			if (id == "")
				return ;
			user = user_table.getUser(id);
		}
        iter it;
        if ((it = channel_tables.find(key)) != channel_tables.end()) {
			channel_tables[key].removeData(user.nick);
			if (channel_tables[key].isEmpty()) {
                channel_tables.erase(it);
            }
        }
		user_table.removeChannel(user, key);
    }

	template<typename T>
    void removeUser(T param) {
		struct s_user_info user;
		if (typeid(T) != typeid(struct s_user_info))
		{
			std::string id = convertFdToId(param);
			if (id == "")
				return ;
			user = user_table.getUser(id);
		}
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
	
	// void removeUserFdint fd) {
	// 	s_user_info user = getUser(convertFdToId(id));
    //     iter it = channel_tables.begin();
    //     while (it != channel_tables.end())
    //     {
    //         iter tmp = it;
	// 		channel_tables[it->first].removeData(user.nick);
    //         if (!channel_tables[it->first].isEmpty()) {
    //             ++it;
    //             channel_tables.erase(tmp);
    //             if (it == channel_tables.end())
    //                 break ;
    //         } else {
    //             ++it;
    //         }
    //     }
    //     user_table.removeUser(user.nick);
    // }

	bool updateUser(struct s_user_info& org, struct s_user_info& usr) {
		if (user_table.isExist(usr.nick))
			return false;
		iter it = channel_tables.begin();
		while (it != channel_tables.end())
        {
			int privileges = channel_tables[it->first].getPrivileges(org.nick);
			if (privileges != -1) {
				channel_tables[it->first].removeData(org.nick);
				channel_tables[it->first].addData(usr);
			}
           	++it;
        }
		user_table.updateUser(org, usr);
		updateFd(org.fd, usr.fd, usr.nick);	
		return true;
	}

	void	updateFd(int org, int new_fd, std::string& new_id) {
		std::map<int, std::string>::iterator iIter = fd_tables.find(org);
		if (iIter != fd_tables.end())	
			fd_tables.erase(iIter);
		fd_tables.insert(std::pair<int, std::string>(new_fd, new_id));
	}

	std::string convertFdToId(int fd)
	{
		std::map<int, std::string>::iterator iIter = fd_tables.find(fd);
		if (iIter != fd_tables.end())	
			return (iIter->second);
		return (static_cast<std::string>(""));
	}
	
	void addChannelUser(struct s_user_info& usr, const std::string& channel_name) {
		if (!user_table.isExist(usr.nick))
		{
			user_table.addUser(usr);
			std::map<std::string, int>::iterator its;
			fd_tables.insert(std::pair<int, std::string>(usr.fd, usr.nick));
		}
		user_table.addChannel(usr, channel_name);
		ChannelData chn = getCorrectChannel(channel_name);
		chn.addData(usr);
	}

	bool addChannelUserFd(int fd, const std::string& channel_name) {
		std::string nick = convertFdToId(fd);
		if (nick == "")
			return (false);
		addChannelUser(user_table.getUser(nick), channel_name);
		return true;
	}

	int getFd(std::string &id) {
		return (user_table.userFd(id));
	}

private:
    std::map<std::string, ChannelData> channel_tables;
	std::map<int, std::string> fd_tables;
    UserData user_table;
};


#endif /* __DB_HPP_ */
