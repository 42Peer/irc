#include "Command.hpp"
#include "userStruct.h"
/*
    4/13 thu (branch)updateCommand 
    Need to update :
        1. remove message command
        2. create pass, user command
        3. update error number
        4. add error rutin
        3. prefix (later)
*/
Command::Command(Handler &Ref_) : _handler(Ref_) {};

std::vector<std::string> getChannelUser(Db &db, std::string name) {
    struct s_user_info user_data;
    user_data = db.getUserTable().getUser(name);
    std::string channel_name = db.getUserTable().getChannelList(user_data);
    return db.getCorrectChannel(channel_name).getUserList();
}

Command::Command(Handler& Ref_) : _handler(Ref_){}

Command::~Command(){}

void Message::run(int fd, std::vector<std::string> args) {
    std::vector<std::string> user_table = getChannelUser(this->_handler.getServer().g_db,
                                                         this->_handler.getServer().getUserName(fd));

    for (size_t j = 0; j < args.size(); ++j) {
        std::cout << args[j] << " ";
    }
}

Message::~Message(){}

void Notice::run(int fd, std::vector<std::string> args) {
    std::vector<std::string>::iterator it = args.begin();
    //prefix
    (void) fd;
    if (args.front()[0] == '#') {
        while (it != args.end() - 1) {
            std::vector<std::string>::iterator cit = this->_handler.getServer().getChannelRef().getUserList(
                    *it).begin();
            std::vector<std::string>::iterator ceit = this->_handler.getServer().getChannelRef().getUserList(*it).end();
            while (cit != ceit) {
                send(this->_handler.getServer().g_db.getUserTable().getUser(*cit).fd, args.back().c_str(),
                     strlen(args.back().c_str()), 0);
                ++cit;
            }
            ++it;
        }
    } else {
        while (it != args.end() - 1) {
            if (this->_handler.getServer().getUserName(
                    this->_handler.getServer().g_db.getUserTable().getUser(*it).fd) == ""); /* err msg */
            else {
                send(this->_handler.getServer().g_db.getUserTable().getUser(*it).fd, args.back().c_str(),
                     strlen(args.back().c_str()), 0);
            }
            ++it;
        }
    }
}

Notice::~Notice(){}

void Join::run(int fd, std::vector<std::string> args) {
    std::vector<std::string>::iterator it = args.begin();
    std::string name = this->_handler.getServer().getUserName(fd);
    std::string msg = "JOIN DONE";
    while (it != args.end()) {
        if (this->_handler.getServer().getChannelRef().setList(*it, name))
            this->_handler.getServer().g_db.addChannel(*it);
        this->_handler.getServer().g_db.addChannelUser(this->_handler.getServer().g_db.getUserTable().getUser(name),
                                                       *it);
        ++it;
    }
    std::cout << "TEST\n";
    send(fd, msg.c_str(), strlen(msg.c_str()), 0);
}

Join::~Join(){}

void Nick::run(int fd, std::vector<std::string> args) {
    std::string old_name = this->_handler.getServer().getUserName(fd);
    std::string new_name = args.front();
    struct s_user_info old_user_info = this->_handler.getServer().g_db.getUserTable().getUser(old_name);
    struct s_user_info new_user_info;

    new_user_info.nick = new_name;
    new_user_info.name = old_user_info.name;
    new_user_info.fd = old_user_info.fd;
    new_user_info.channel_list = old_user_info.channel_list;

    if (!this->_handler.getServer().g_db.updateUser(old_user_info, new_user_info)) {
        //error_sending
        // const char* msg = "Duplicated nick";
        // send(fd, msg, strlen(msg), 0);
        /*
            에러넘버를 세팅할지 말지 (논의 필요)
        */
        return;
    }
    std::vector<std::string>::iterator it = this->_handler.getServer().g_db.getUserTable().getUser(
            new_name).channel_list.begin();
    std::vector<std::string>::iterator eit = this->_handler.getServer().g_db.getUserTable().getUser(
            new_name).channel_list.end();
    while (it != eit) {
        this->_handler.getServer().getChannelRef().changeNameFromChannelList(*it, old_name, new_name);
        ++it;
    }
    this->_handler.getServer().setMapData(fd, new_name);
}

Nick::~Nick(){}

void Quit::run(int fd, std::vector<std::string> args) {
    (void) args;
    std::string usr_name = this->_handler.getServer().getUserName(fd);
    struct s_user_info usr_name_info = this->_handler.getServer().g_db.getUserTable().getUser(usr_name);
    std::vector<std::string>::iterator it = usr_name_info.channel_list.begin();
    while (it != usr_name_info.channel_list.end()) {
        this->_handler.getServer().getChannelRef().removeUserFromChannel(*it, usr_name);
        ++it;
    }
    this->_handler.getServer().g_db.removeUser(usr_name_info);
    this->_handler.getServer().removeMapData(fd);
    close(fd);
}

Quit::~Quit(){}

void Privmsg::run(int fd, std::vector<std::string> args) {
    (void) fd;
    std::vector<std::string>::iterator it = args.begin();
    std::string msg = "Wrong\n";
    std::cout << "DOon\n";
    while (it != args.end() - 1) {
        if (this->_handler.getServer().g_db.getUserTable().isExist(*it))
            send(this->_handler.getServer().g_db.getUserTable().getUser(*it).fd, args.back().c_str(),
                 strlen(args.back().c_str()), 0);
        else /* error msg */
            send(fd, msg.c_str(), strlen(msg.c_str()), 0);
        ++it;
    }
}

Privmsg::~Privmsg(){}

void Kick::run(int fd, std::vector<std::string> args) {
    std::string name = this->_handler.getServer().getUserName(fd);
    if (this->_handler.getServer().g_db.getUserTable().getUser(name).channel_list.size() <= 0)
        return; /* error msg */
    std::string kicked_name = args.front();
    std::string current_channel = this->_handler.getServer().g_db.getUserTable().getUser(name).channel_list.back();
    if (!this->_handler.getServer().g_db.getUserTable().isExist(kicked_name))
        return; /* error msg */
    else if (!(this->_handler.getServer().getChannelRef().isExistInChannel(current_channel, kicked_name)))
        return; /* error msg */
    if (this->_handler.getServer().g_db.getCorrectChannel(
            this->_handler.getServer().g_db.getUserTable().getUser(name).channel_list.back()).getPrivileges(name) !=
        -1) {
        this->_handler.getServer().g_db.getUserTable().removeChannel(
                this->_handler.getServer().g_db.getUserTable().getUser(kicked_name), current_channel);
        this->_handler.getServer().getChannelRef().removeUserFromChannel(current_channel, kicked_name);
        // send(this->_handler.getServer().g_db.getUserTable().getUser(kicked_name).fd,) /* send kicked msg */
    } else
        return; /* error msg */
}

Kick::~Kick(){}

void Part::run(int fd, std::vector<std::string> args) {
    std::string name = this->_handler.getServer().getUserName(fd);

    if (!this->_handler.getServer().g_db.getUserTable().getUser(name).channel_list.size())
        return; /* error msg */
    else if (!args.size()) {
        std::string current_channel = this->_handler.getServer().g_db.getUserTable().getUser(name).channel_list.back();
        this->_handler.getServer().g_db.getUserTable().removeChannel(
                this->_handler.getServer().g_db.getUserTable().getUser(name), current_channel);
    } else {
        std::vector<std::string>::iterator it = args.begin();
        while (it != args.end()) {
            std::vector<std::string>::iterator fit = this->_handler.getServer().g_db.getUserTable().getUser(
                    name).channel_list.begin();
            std::vector<std::string>::iterator feit = this->_handler.getServer().g_db.getUserTable().getUser(
                    name).channel_list.end();
            std::vector<std::string>::iterator fpos = std::find(fit, feit, *it);
            if (fpos != feit)
                this->_handler.getServer().g_db.getUserTable().removeChannel(
                        this->_handler.getServer().g_db.getUserTable().getUser(name), *it);
            else { ; /* send err msg */
            }
            ++it;
        }
    }
}

Part::~Part(){}