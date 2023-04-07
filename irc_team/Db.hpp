#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <map>
#include <string>
#include <utility>

using namespace std;

struct s_user_info {
    int fd;
    bool privileges;
//    std::string nick;
    std::string name;
};


class ChannelData
{
public:
    typedef std::map<std::string, s_user_info>::iterator iter;
    bool isExist(std::string nick) {
        if (_tables.find(nick) != _tables.end())
            return (true);
        return (false);
    }

    bool addData(bool privilege, int fd, std::string nick, std::string name) {
        if (isExist(nick))
            return false;
        struct s_user_info info;

        info.privileges = privilege;
        info.fd = fd;
        info.name = name;
        _tables.insert(std::pair<std::string, struct s_user_info>(nick, info));
        return true;
    }

    void printTables() {
        for (iter it = _tables.begin(); it != _tables.end(); ++it) {
            if (it->second.privileges == 0)
                std::cout << "방장";
            else
                std::cout << "User";
            std::cout << "[user name : " << it->second.name << "]";
            std::cout << "[user nick : " << it->first << "]\n";
        }
    }

private:
    std::map<std::string, struct s_user_info> _tables;
};

/*          */
class UserData
{
public:

private:
};

class Db
{
public:
    UserData&   getUserTable() {
        return (user_table);
    }
    void addTable() {}
private:
    std::map<std::string, ChannelData> channel_tables;
    UserData user_table;
};


#endif /* __DB_HPP_ */
