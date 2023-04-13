#include "Command.hpp"
#include "userStruct.h"
/*
    4/13 thu (branch)updateCommand 
    Need to update :
        1. remove message command [v]
        3. add error number part 
        4. add error routine
        3. prefix (later)
    NOTICE JOIN NICK QUIT PRIVMSG KICK PART
    what they give you : cmdFigure(fd, vector<string>)
    and we will send(BUF)
    jujeon : nickname, hi : username
    BUF should be shape of "<nickname> <command> :<channel> :<message>"
    "jujeon PRIVMSG :#999 :kick #123 hello"
    in error case BUF should be shape of "476 jujeon 123 :Invalid channel name"

    메세지가 있는 커맨드의 경우 v.back()으로 바로 메세지에 접근해서 메세지인지 확인해야함.
    만약 옵셔널인자가 있는경우 v.size()로 먼저 인자의 갯수를 보고 메세지가 있는지 판단한다. 
*/
Command::Command(Handler &Ref_) : _handler(Ref_) {};

Command::~Command(){}

std::vector<std::string> getChannelUser(Db &db, std::string name) {
    struct s_user_info user_data;
    user_data = db.getUserTable().getUser(name);
    std::string channel_name = db.getUserTable().getChannelList(user_data);
    return db.getCorrectChannel(channel_name).getUserList();
}

// void Message::run(int fd, std::vector<std::string> args) {
//     std::vector<std::string> user_table = getChannelUser(this->_handler.getServer().g_db,
//                                                          this->_handler.getServer().getUserName(fd));

//     for (size_t j = 0; j < args.size(); ++j) {
//         std::cout << args[j] << " ";
//     }
// }

// Message::~Message(){}

void Notice::run(int fd, std::vector<std::string> args) {
    /*
        notice : <nickname> <text> 
            jujeon NOTICE root_ :hi

        vector<string> args : 
            2개의 인자(nickname, text)가 들어온다.
            (1) nickname이 ','로 연결되어잇을 수 있다.
                (1-1) 닉네임이 없으면 401 에러
                (1-2) 있다면 여러개의 인자를 못받으므로 407 에러
                (1-3) '#' 이 있다면 채널이니까 채널에는 notice 안되므로 404 에러
            (2) text만 들어온다. 따로 처리해줄 필요없다
            (3) nickname NOTICE 대상닉네임 :text로 send()한다
 
        error case
            user1,user1 :msg 407 "<target> :Duplicate recipients. No message delivered"
            user1 :msg 401 "<nickname>: No such (user1)nick/channel"
            #channel :msg 404 "<channel name> :Cannot send to channel"
     */

    // std::vector<std::string>::iterator it = args.begin();
    // //prefix
    // (void) fd;
    // if (args.front()[0] == '#') {
    //     while (it != args.end() - 1) {
    //         std::vector<std::string>::iterator cit = this->_handler.getServer().getChannelRef().getUserList(
    //                 *it).begin();
    //         std::vector<std::string>::iterator ceit = this->_handler.getServer().getChannelRef().getUserList(*it).end();
    //         while (cit != ceit) {
    //             send(this->_handler.getServer().g_db.getUserTable().getUser(*cit).fd, args.back().c_str(),
    //                  strlen(args.back().c_str()), 0);
    //             ++cit;
    //         }
    //         ++it;
    //     }
    // } else {
    //     while (it != args.end() - 1) {
    //         if (this->_handler.getServer().getUserName(
    //                 this->_handler.getServer().g_db.getUserTable().getUser(*it).fd) == ""); /* err msg */
    //         else {
    //             send(this->_handler.getServer().g_db.getUserTable().getUser(*it).fd, args.back().c_str(),
    //                  strlen(args.back().c_str()), 0);
    //         }
    //         ++it;
    //     }
    // }

    std::vector<std::string>::iterator it = args.begin();
    std::string buf;
    //	404 ERR_CANNOTSENDTOCHAN "<channel name> :Cannot send to channel"
    if (args.front()[0] == '#') {
        buf += "404 ";
        buf += args.front();
        buf += " :Cannot send to channel";
        send(fd, buf.c_str(), buf.size(), 0);
    }                
    // 401 ERR_NOSUCHNICK " <nickname>: No such (user1)nick/channel"
    else if (this->_handler.getServer().g_db.getUserTable().isExist(args.front()) == false){
       buf += "401 ";
       buf += args.front();
       buf += ": No such nickname";
       send(fd, buf.c_str(), buf.size(), 0);
    }
    //nickname NOTICE 대상닉네임 :text
    else {
        buf += this->_handler.getServer().getUserName(fd);
        buf += " NOTICE ";
        buf += args.front();
		buf += " :";
		buf += args.back();
        send(this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd, buf.c_str(), buf.size(), 0);
    }
}

Notice::~Notice(){}

void Join::run(int fd, std::vector<std::string> args) {
	/*
		join <channel>[,<channel>]: 
			jujeon JOIN :#999
		
		(1) iterator 로 args 다 돌려서 join 시키면 된다.
		(2) channel mask 검사
			(2-1) 없다면 
		(2) args 채널에 내가 있는지 검사
			

 
		error case
			476 jujeon 123 :Invalid channel name		
			ERR_BADCHANMASK (476) "<channel> :Bad Channel Mask"
	*/

//     std::vector<std::string>::iterator it = args.begin();
//     std::string name = this->_handler.getServer().getUserName(fd);
//     std::string msg = "JOIN DONE";
//     while (it != args.end()) {
//         if (this->_handler.getServer().getChannelRef().setList(*it, name))
//             this->_handler.getServer().g_db.addChannel(*it);
//         this->_handler.getServer().g_db.addChannelUser(this->_handler.getServer().g_db.getUserTable().getUser(name),
//                                                        *it);
//         ++it;
//     }
//     std::cout << "TEST\n";
//     send(fd, msg.c_str(), strlen(msg.c_str()), 0);
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