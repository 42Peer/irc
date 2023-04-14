#include "Db.hpp"

int ChannelData::grantUser() const { return (_tables.size() >= 1); }

bool ChannelData::isExist(const std::string &nick) const {
    return (_tables.find(nick) != _tables.end());
}

int ChannelData::getPrivileges(const std::string &nick) {
    if (isExist(nick)) {
        return (_tables[nick]);
    }
    return (-1);
}

bool ChannelData::addData(struct s_user_info &user) {
    if (isExist(user.nick)) {
        return false;
    }
    int privileges = grantUser();
    _tables.insert(std::pair<std::string, int>(user.nick, privileges));
    return true;
}

bool ChannelData::isEmpty() const { return (!_tables.empty()); }

void ChannelData::removeData(const std::string &nick) {
    std::map<std::string, int>::iterator it = _tables.find(nick);
    if (it != _tables.end())
        _tables.erase(it);
}

std::vector<std::string> ChannelData::getUserList() {
    std::map<std::string, int>::iterator it = _tables.begin();
    std::vector<std::string> userList;
    for (; it != _tables.end(); ++it)
        userList.push_back(it->first);
    return userList;
}

bool UserData::isExist(const std::string &nick) const {
    return (_tables.find(nick) != _tables.end());
}

bool UserData::addUser(struct s_user_info &user) {
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

void UserData::updateUser(struct s_user_info org, struct s_user_info usr) {
    usr.channel_list.resize(org.channel_list.size());
    for (size_t i = 0; i < org.channel_list.size(); ++i) {
        usr.channel_list[i] = org.channel_list[i];
    }
    if (isExist(org.nick))
        _tables.erase(org.nick);
    usr.channel_list = org.channel_list;
    _tables.insert(std::pair<std::string, struct s_user_info>(usr.nick, usr));
}

void UserData::removeUser(std::string key) { _tables.erase(key); }

void UserData::addChannel(struct s_user_info &usr, const std::string &channel_name) {
    if (isExist(usr.nick)) {
        for (size_t i = 0; i < _tables[usr.nick].channel_list.size(); ++i) {
            if (_tables[usr.nick].channel_list[i] == channel_name) {
                _tables[usr.nick].channel_list.erase(
                        _tables[usr.nick].channel_list.begin() + i
                );
            }
        }
    }
    _tables[usr.nick].channel_list.push_back(channel_name);
}

void UserData::removeChannel(struct s_user_info &usr, const std::string &channel_name) {
    std::vector<std::string>::iterator viter =
            _tables[usr.nick].channel_list.begin();

    for (; viter != _tables[usr.nick].channel_list.end(); ++viter) {
        if (*viter == channel_name) {
            _tables[usr.nick].channel_list.erase(viter);
            return;
        }
    }
}

std::string UserData::getChannelList(struct s_user_info &usr) {
    if (isExist(usr.nick)) {
        iter it = _tables.find(usr.nick);
        if (it->second.channel_list.empty())
            return static_cast<std::string>("");
        return (it->second.channel_list[it->second.channel_list.size() - 1]);
    }
    return static_cast<std::string>("");
}

s_user_info& UserData::getUser(const std::string &id) { return (_tables[id]); }

ChannelData& Db::getCorrectChannel(const std::string &channelName) {
    if (channel_tables.find(channelName) == channel_tables.end()) {
        addChannel(channelName);
    }
    return (channel_tables[channelName]);
}

bool Db::isExist(const std::string &id) { return (user_table.isExist(id)); }

bool Db::addChannel(const std::string &cname) {
    if (channel_tables.find(cname) == channel_tables.end()) {
        ChannelData chn;
        channel_tables.insert(std::pair<std::string, ChannelData>(cname, chn));
        return (true);
    }
    return (false);
}

UserData& Db::getUserTable() { return (user_table); }

void Db::addUser(s_user_info &user) {
    if (user_table.isExist(user.nick))
        return;
    user_table.addUser(user);
}

void Db::removeChannel(struct s_user_info &user, const std::string &key) {
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

void Db::removeUser(struct s_user_info &user) {
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

bool Db::updateUser(struct s_user_info &org, struct s_user_info &usr) {
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

void Db::addChannelUser(struct s_user_info &usr,
                    const std::string &channel_name) {
    if (!user_table.isExist(usr.nick)) {
        user_table.addUser(usr);
    }
    user_table.addChannel(usr, channel_name);
    getCorrectChannel(channel_name).addData(usr);
}