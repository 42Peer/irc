#ifndef DB_HPP
#define DB_HPP

#include <iostream>
#include <map>
#include <vector>

using namespace std;

class ChannelData
{
public:
    std::string getAllUser() {
        std::string ret;

        for (int i = 0; i < _user_list.size(); ++i) {
            ret += _user_list[i] + "\n";
        }
        return (ret);
    }

    void setUser(std::string username, int fd, bool privilge) {
        _user_list.push_back(username);
        _is_super.push_back(privilge);
        _fd.push_back(fd);
    }

private:
    vector<string>  _user_list;
    vector<bool>    _is_super;
    vector<int>     _fd;
};
/*          */

class UserData
{

};

class Db
{
public:
    map<std::string, ChannelData> channel_tables;
};


#endif /* __DB_HPP_ */
