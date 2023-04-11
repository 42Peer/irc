#ifndef DB_HPP
#define DB_HPP

#include "userStruct.h"

class ChannelData {
public:
  typedef std::map<std::string, int>::iterator iter;

  int grantUser() const { return (_tables.size() >= 1); }

  bool isExist(const std::string &nick) const {
    return (_tables.find(nick) != _tables.end());
  }

  int getPrivileges(const std::string &nick) {
    if (isExist(nick)) {
      return (_tables[nick]);
    }
    return (-1);
  }

  bool addData(struct s_user_info &user) {
    if (isExist(user.nick)) {
      return false;
    }
    int privileges = grantUser();
    _tables.insert(std::pair<std::string, int>(user.nick, privileges));
    return true;
  }

  bool isEmpty() const { return (!_tables.empty()); }

  void removeData(const std::string &nick) {
    iter it = _tables.find(nick);
    if (it != _tables.end())
      _tables.erase(it);
  }

private:
  std::map<std::string, int> _tables;
};

class UserData {
public:
  typedef std::map<std::string, struct s_user_info>::iterator iter;
  bool isExist(const std::string &nick) const {
    return (_tables.find(nick) != _tables.end());
  }

  bool addUser(struct s_user_info &user) {
    if (isExist(user.nick))
      return (false);
    struct s_user_info info;

    info.name = user.name;
    info.nick = user.nick;
    info.fd = user.fd;
    info.channel_list = user.channel_list;
    _tables.insert(std::pair<std::string, struct s_user_info>(user.nick, info));
    return (true);
  }

  void updateUser(struct s_user_info org, struct s_user_info usr) {
    usr.channel_list.resize(org.channel_list.size());
    for (size_t i = 0; i < org.channel_list.size(); ++i) {
        usr.channel_list[i] = org.channel_list[i];
    }
    if (isExist(org.nick))
        _tables.erase(org.nick);
    usr.channel_list = org.channel_list;
    _tables.insert(std::pair<std::string, struct s_user_info>(usr.nick, usr));
  }

  void removeUser(std::string key) { _tables.erase(key); }

  void addChannel(struct s_user_info &usr, const std::string &channel_name) {
    std::vector<std::string>::iterator sitor = _tables[usr.nick].channel_list.begin();
    for (;sitor != _tables[usr.nick].channel_list.end(); ++sitor)
        if (*sitor == channel_name)
            _tables[usr.nick].channel_list.erase(sitor);
    _tables[usr.nick].channel_list.push_back(channel_name);
  }

  void removeChannel(struct s_user_info &usr, const std::string &channel_name) {
    std::vector<std::string>::iterator viter =
        _tables[usr.nick].channel_list.begin();

    for (; viter != _tables[usr.nick].channel_list.end(); ++viter) {
      if (*viter == channel_name) {
        _tables[usr.nick].channel_list.erase(viter);
        return;
      }
    }
  }

  std::string getChannelList(struct s_user_info &usr) {
    if (isExist(usr.nick)) {
      iter it = _tables.find(usr.nick);
      if (it->second.channel_list.empty())
        return static_cast<std::string>("");
      return (it->second.channel_list[it->second.channel_list.size() - 1]);
    }
    return static_cast<std::string>("");
  }

  s_user_info &getUser(const std::string &id) { return (_tables[id]); }

private:
  std::map<std::string, struct s_user_info> _tables;
};

class Db {
public:
  typedef std::map<std::string, ChannelData>::iterator iter;

  ChannelData &getCorrectChannel(const std::string &channelName) {
    if (channel_tables.find(channelName) == channel_tables.end()) {
      addChannel(channelName);
    }
    return (channel_tables[channelName]);
  }

  bool isExist(const std::string &id) { return (user_table.isExist(id)); }

  bool addChannel(const std::string &cname) {
    if (channel_tables.find(cname) == channel_tables.end()) {
      ChannelData chn;
      channel_tables.insert(std::pair<std::string, ChannelData>(cname, chn));
      return (true);
    }
    return (false);
  }

  UserData &getUserTable() { return (user_table); }

  void addUser(s_user_info &user) {
    if (user_table.isExist(user.nick))
      return;
    user_table.addUser(user);
  }

  void removeChannel(struct s_user_info &user, const std::string &key) {
    /* iterator erase*/
    iter it;
    if ((it = channel_tables.find(key)) != channel_tables.end()) {
      if (channel_tables[key].isExist(user.nick)) {
        channel_tables[key].removeData(user.nick);
      }
    }
    user_table.removeChannel(user, key);
    std::map<std::string, ChannelData>::iterator channelIter = channel_tables.find(key);
    if (channelIter != channel_tables.end() && channelIter->second.isEmpty())
       channel_tables.erase(channelIter);
  }

  void removeUser(struct s_user_info &user) {
    iter it = channel_tables.begin();
    while (it != channel_tables.end()) {
      iter tmp = it;
      channel_tables[it->first].removeData(user.nick);
      if (!channel_tables[it->first].isEmpty()) {
        ++it;
        channel_tables.erase(tmp);
        if (it == channel_tables.end())
          break;
      } else {
        ++it;
      }
    }
    user_table.removeUser(user.nick);
  }

  bool updateUser(struct s_user_info &org, struct s_user_info &usr) {
    if (user_table.isExist(usr.nick))
      return false;
    user_table.updateUser(org, usr);
    iter it = channel_tables.begin();
    while (it != channel_tables.end()) {
      if (it->second.isExist(org.nick)) {
        channel_tables[it->first].removeData(org.nick);
        channel_tables[it->first].addData(usr);
      }
      ++it;
    }
    return true;
  }

  void addChannelUser(struct s_user_info &usr,
                      const std::string &channel_name) {
    if (!user_table.isExist(usr.nick)) {
      user_table.addUser(usr);
    }
    user_table.addChannel(usr, channel_name);
    getCorrectChannel(channel_name).addData(usr);
  }

	private:
		std::map<std::string, ChannelData> channel_tables;
		UserData user_table;
};

#endif /* __DB_HPP_ */