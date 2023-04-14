#ifndef DB_HPP
#define DB_HPP

#include "userStruct.h"

class ChannelData {
public:
    int grantUser() const;

    bool isExist(const std::string &nick) const ;

    int getPrivileges(const std::string &nick);

    bool addData(struct s_user_info &user) ;

    bool isEmpty() const;

    void removeData(const std::string &nick) ;

    std::vector<std::string> getUserList() ;

private:
    std::map<std::string, int> _tables;
};

class UserData {
public:
    typedef std::map<std::string, struct s_user_info>::iterator iter;

    bool isExist(const std::string &nick) const ;

    bool addUser(struct s_user_info &user) ;

    void updateUser(struct s_user_info org, struct s_user_info usr) ;

    void removeUser(std::string key) ;

    void addChannel(struct s_user_info &usr, const std::string &channel_name) ;

    void removeChannel(struct s_user_info &usr, const std::string &channel_name);

    std::string getChannelList(struct s_user_info &usr);

    s_user_info &getUser(const std::string &id);

private:
    std::map<std::string, struct s_user_info> _tables;
};

class Db {
public:
    typedef std::map<std::string, ChannelData>::iterator iter;

    ChannelData &getCorrectChannel(const std::string &channelName);

    bool isExist(const std::string &id);

    bool addChannel(const std::string &cname);

    UserData &getUserTable();

    void addUser(s_user_info &user);

    void removeChannel(struct s_user_info &user, const std::string &key);

    void removeUser(struct s_user_info &user);

    bool updateUser(struct s_user_info &org, struct s_user_info &usr);

    void addChannelUser(struct s_user_info &usr,
                        const std::string &channel_name);

private:
    std::map<std::string, ChannelData> channel_tables;
    UserData user_table;
};

#endif /* __DB_HPP_ */