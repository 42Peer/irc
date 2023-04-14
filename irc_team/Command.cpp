#include "Command.hpp"
#include "Db.hpp"
#include "userStruct.h"

#define ERR461 "461 :Not enough parameters:"
#define ERR476 "476 :Invalid channel name:"
#define ERR401 "401 :No such nick or channel:"
#define ERR404 "404 :Cannot send to channel:"
#define ERR433 "433 :Nickname is already in use:"
#define ERR407 "407 :Duplicate recipients. No message delivered:"
#define ERR412 "412 :No text to send:"
#define ERR403 "403 :No such channel:"
#define ERR442 "442 :You're not on that channel:"
#define ERR462 "462 :You may not reregister:"

#define MSGNOTICE "NOTICE :"
#define MSGJOIN "JOIN :"
#define MSGNICK "NICK :"
#define MSGQUIT "QUIT :"
#define MSGPRIVMSG "PRIVMSG :"
#define MSGKICK "NICK :"
#define MSGPART "PART :"
#define MSGPART "PART :"

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

    BUF should be shape of "<command> :<channel> :<message>"
    "PRIVMSG :#999 :kick #123 hello"

    in error case BUF should be shape of "476 :Invalid channel name: 123"

    메세지가 있는 커맨드의 경우 v.back()으로 바로 메세지에 접근해서 메세지인지
   확인해야함. 만약 옵셔널인자가 있는경우 v.size()로 먼저 인자의 갯수를 보고
   메세지가 있는지 판단한다.
*/

Command::Command(Handler &Ref_) : _handler(Ref_) {}

Command::~Command() {}

std::vector<std::string> getChannelUser(Db &db, std::string name) {
  struct s_user_info user_data;
  user_data = db.getUserTable().getUser(name);
  std::string channel_name = db.getUserTable().getChannelList(user_data);
  return db.getCorrectChannel(channel_name).getUserList();
}

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
          user1,user1 :msg 407 "<target> :Duplicate recipients. No message
     delivered" user1 :msg 401 "<nickname>: No such (user1)nick/channel"
          #channel :msg 404 "<channel name> :Cannot send to channel"
   */

  // std::vector<std::string>::iterator it = args.begin();
  // //prefix
  // (void) fd;
  // if (args.front()[0] == '#') {
  //     while (it != args.end() - 1) {
  //         std::vector<std::string>::iterator cit =
  //         this->_handler.getServer().getChannelRef().getUserList(
  //                 *it).begin();
  //         std::vector<std::string>::iterator ceit =
  //         this->_handler.getServer().getChannelRef().getUserList(*it).end();
  //         while (cit != ceit) {
  //             send(this->_handler.getServer().g_db.getUserTable().getUser(*cit).fd,
  //             args.back().c_str(),
  //                  strlen(args.back().c_str()), 0);
  //             ++cit;
  //         }
  //         ++it;
  //     }
  // } else {
  //     while (it != args.end() - 1) {
  //         if (this->_handler.getServer().getUserName(
  //                 this->_handler.getServer().g_db.getUserTable().getUser(*it).fd)
  //                 == ""); /* err msg */
  //         else {
  //             send(this->_handler.getServer().g_db.getUserTable().getUser(*it).fd,
  //             args.back().c_str(),
  //                  strlen(args.back().c_str()), 0);
  //         }
  //         ++it;
  //     }
  // }

  std::vector<std::string>::iterator it = args.begin();
  std::string buf;
  //	404 ERR_CANNOTSENDTOCHAN "<channel name> :Cannot send to channel"
  if (args.front()[0] == '#') {
    buf.append(ERR404);
    buf.append(args.front());
    send(fd, buf.c_str(), buf.size(), 0);
  }
  // 401 ERR_NOSUCHNICK " <nickname>: No such (user1)nick/channel"
  else if (this->_handler.getServer().g_db.getUserTable().isExist(
               args.front()) == false) {
    buf.append(ERR401);
    buf.append(args.front());
    send(fd, buf.c_str(), buf.size(), 0);
  }
  // nickname NOTICE 대상닉네임 :text
  else {
    buf += this->_handler.getServer().getUserName(fd);
    buf.append(MSGNOTICE);
    buf.append(args.front());
    buf.append(" :");
    buf.append(args.back());
    send(
        this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd,
        buf.c_str(), buf.size(), 0);
  }
}

Notice::~Notice() {}

void Join::run(int fd, std::vector<std::string> args) {
  /*
          join <channel>[,<channel>]: 그런데 우리는 args에 쪼개져서 들어온다
                  jujeon JOIN :#999

          (1) iterator 로 args 루프문으로 돌리면서 처리할 것이다
          (2) channel mask 검사
                  (2-1) 없다면 476 에러 반환
          (3) 들어가려는 채널에 이미 내가 있는지 검사
                  (3-1) 없다면 채널 들어간다.
  (4) 그리고 나와 채널에 속한 사람들에게 메세지 출력

          error case
                  476 jujeon 123 :Invalid channel name
                  ERR_BADCHANMASK (476) "<channel> :Bad Channel Mask"
  */

  // std::vector<std::string>::iterator it = args.begin();
  // std::string name = this->_handler.getServer().getUserName(fd);
  // std::string msg = "JOIN DONE";
  // while (it != args.end()) {
  //     if (this->_handler.getServer().getChannelRef().setList(*it, name))
  //         this->_handler.getServer().g_db.addChannel(*it);
  //     this->_handler.getServer().g_db.addChannelUser(this->_handler.getServer().g_db.getUserTable().getUser(name),
  //                                                    *it);
  //     ++it;
  // }
  // std::cout << "TEST\n";
  // send(fd, msg.c_str(), strlen(msg.c_str()), 0);

  std::vector<std::string>::iterator it_channel_name = args.begin();
  std::vector<std::string>::iterator eit = args.end();
  std::string my_nick_name = this->_handler.getServer().getUserName(fd);
  std::string buf;
  for (; it_channel_name != eit; ++it_channel_name) {
    size_t pos = it_channel_name->find('#');
    if (pos != std::string::npos) {
      // 476 :Invalid channel name: 123
      buf.append(ERR476);
      buf.append(*it_channel_name);
      send(fd, buf.c_str(), buf.size(), 0);
      return;
    } else {
      std::string nick_name = this->_handler.getServer().getUserName(fd);
      // chanel, server map 변수 업데이트 해줘야함.
      if (this->_handler.getServer().getChannelRef().isExistInChannel(
              *it_channel_name, my_nick_name) == false) {
        this->_handler.getServer().getChannelRef().setList(*it_channel_name,
                                                           my_nick_name);
        this->_handler.getServer().g_db.addChannelUser(
            this->_handler.getServer().g_db.getUserTable().getUser(nick_name),
            *it_channel_name);
        // jujeon JOIN :#999
        buf.append(nick_name);
        buf.append(MSGJOIN);
        buf.append(*it_channel_name);
        send(fd, buf.c_str(), buf.size(), 0);
        // 채널에 속한 유저들에게 똑같은 메세지를 send() 해줘야함
        std::vector<std::string>::iterator it_all_user_list =
            this->_handler.getServer()
                .getChannelRef()
                .getUserList(*it_channel_name)
                .begin();
        std::vector<std::string>::iterator eit_all_user_list =
            this->_handler.getServer()
                .getChannelRef()
                .getUserList(*it_channel_name)
                .end();
        for (; it_all_user_list < eit_all_user_list; ++it_all_user_list) {
          int receiver = this->_handler.getServer()
                             .g_db.getUserTable()
                             .getUser(*it_channel_name)
                             .fd;
          send(receiver, buf.c_str(), buf.size(), 0);
        }
      }
    }
  }
}

Join::~Join() {}

void Nick::run(int fd, std::vector<std::string> args) {
  /*
          nick <nickname> :
                  NICK :hello

		  (0) 닉네임에 #이 존재하는지 체크
		  	(0-1) #이 존재하면 에러
          (1) db에 해당 닉네임 이미 존재하는지 체크
                  (1-1) 존재하면 433 에러
                  (1-2) 존재하지 않으면 서버랑, 채널의 map 에 닉네임 변경해주기
                          (1-2-1) 그리고, db에 닉네임도 변경해주기
          (2) NICK :hello 메세지 띄워주기

          error case
                  :irc.local 433 jujeon root_ :Nickname is already in use.
                  433     ERR_NICKNAMEINUSE "<nick> :Nickname is already in use"

  */
  // std::string old_name = this->_handler.getServer().getUserName(fd);
  // std::string new_name = args.front();
  // struct s_user_info old_user_info =
  // this->_handler.getServer().g_db.getUserTable().getUser(old_name); struct
  // s_user_info new_user_info;

  // new_user_info.nick = new_name;
  // new_user_info.name = old_user_info.name;
  // new_user_info.fd = old_user_info.fd;
  // new_user_info.channel_list = old_user_info.channel_list;

  // if (!this->_handler.getServer().g_db.updateUser(old_user_info,
  // new_user_info)) {
  //     //error_sending
  //     // const char* msg = "Duplicated nick";
  //     // send(fd, msg, strlen(msg), 0);
  //     /*
  //         에러넘버를 세팅할지 말지 (논의 필요)
  //     */
  //     return;
  // }
  // std::vector<std::string>::iterator it =
  // this->_handler.getServer().g_db.getUserTable().getUser(
  //         new_name).channel_list.begin();
  // std::vector<std::string>::iterator eit =
  // this->_handler.getServer().g_db.getUserTable().getUser(
  //         new_name).channel_list.end();
  // while (it != eit) {
  //     this->_handler.getServer().getChannelRef().changeNameFromChannelList(*it,
  //     old_name, new_name);
  //     ++it;
  // }
  // this->_handler.getServer().setMapData(fd, new_name);

  // std::string buf;
  // if (this->_handler.getServer().g_db.getUserTable().isExist(args.front())) {
  // 	buf.append(ERR433);
  // 	buf.append(args.front());
  // 	send(fd, buf.c_str(), buf.size(), 0);
  // 	return ;
  // }
  // else {
  // 	this->_handler.getServer().setMapData(fd, args.front());
  // 	for (;;) {
  // 		this->_handler.getServer().getChannelRef().changeNameFromChannelList(,
  // std::string, std::string)
  // 	}
  // }
}

Nick::~Nick() {}

void Quit::run(int fd, std::vector<std::string> args) {
  (void)args;
  std::string usr_name = this->_handler.getServer().getUserName(fd);
  struct s_user_info usr_name_info =
      this->_handler.getServer().g_db.getUserTable().getUser(usr_name);
  std::vector<std::string>::iterator it = usr_name_info.channel_list.begin();
  while (it != usr_name_info.channel_list.end()) {
    this->_handler.getServer().getChannelRef().removeUserFromChannel(*it,
                                                                     usr_name);
    ++it;
  }
  this->_handler.getServer().g_db.removeUser(usr_name_info);
  this->_handler.getServer().removeMapData(fd);
  close(fd);
}

Quit::~Quit() {}

void Privmsg::run(int fd, std::vector<std::string> args) {
  (void)fd;
  std::vector<std::string>::iterator it = args.begin();
  std::string msg = "";
  std::cout << "DOon\n";
  Db &db = this->_handler.getServer().g_db;
  UserData user = db.getUserTable();
  while (it != args.end() - 1) {
    if (user.isExist(*it))
      send(user.getUser(*it).fd, args.back().c_str(),
           strlen(args.back().c_str()), 0);
    else {
        ChannelData chn = db.getCorrectChannel(*it);
        std::vector<std::string> channel_user =
            db.getCorrectChannel(*it).getUserList();
        if (channel_user.size() <= 0) {
            msg += ERR 
        }
      // send(fd, msg.c_str(), strlen(msg.c_str()), 0);
    }
    ++it;
  }
}

Privmsg::~Privmsg() {}

void Kick::run(int fd, std::vector<std::string> args) {
  std::string name = this->_handler.getServer().getUserName(fd);
  if (this->_handler.getServer()
          .g_db.getUserTable()
          .getUser(name)
          .channel_list.size() <= 0)
    return; /* error msg */
  std::string kicked_name = args.front();
  std::string current_channel = this->_handler.getServer()
                                    .g_db.getUserTable()
                                    .getUser(name)
                                    .channel_list.back();
  if (!this->_handler.getServer().g_db.getUserTable().isExist(kicked_name))
    return; /* error msg */
  else if (!(this->_handler.getServer().getChannelRef().isExistInChannel(
               current_channel, kicked_name)))
    return; /* error msg */
  if (this->_handler.getServer()
          .g_db
          .getCorrectChannel(this->_handler.getServer()
                                 .g_db.getUserTable()
                                 .getUser(name)
                                 .channel_list.back())
          .getPrivileges(name) != -1) {
    this->_handler.getServer().g_db.getUserTable().removeChannel(
        this->_handler.getServer().g_db.getUserTable().getUser(kicked_name),
        current_channel);
    this->_handler.getServer().getChannelRef().removeUserFromChannel(
        current_channel, kicked_name);
    // send(this->_handler.getServer().g_db.getUserTable().getUser(kicked_name).fd,)
    // /* send kicked msg */
  } else
    return; /* error msg */
}

Kick::~Kick() {}

void Part::run(int fd, std::vector<std::string> args) {
  std::string name = this->_handler.getServer().getUserName(fd);

  if (!this->_handler.getServer()
           .g_db.getUserTable()
           .getUser(name)
           .channel_list.size()) {
    // the user is not in any channel
    // 442     ERR_NOTONCHANNEL "<channel> :You're not on that channel"
    _handler.getMsgBuf(fd).append(ERR442);
    return;
  } else if (!args.size()) {
    // no arg -> exit current channel
    std::string current_channel = this->_handler.getServer()
                                      .g_db.getUserTable()
                                      .getUser(name)
                                      .channel_list.back();
    this->_handler.getServer().g_db.getUserTable().removeChannel(
        this->_handler.getServer().g_db.getUserTable().getUser(name),
        current_channel);
  } else {
    std::vector<std::string>::iterator it = args.begin();
    while (it != args.end()) {
      std::vector<std::string>::iterator fit = this->_handler.getServer()
                                                   .g_db.getUserTable()
                                                   .getUser(name)
                                                   .channel_list.begin();
      std::vector<std::string>::iterator feit = this->_handler.getServer()
                                                    .g_db.getUserTable()
                                                    .getUser(name)
                                                    .channel_list.end();
      std::vector<std::string>::iterator fpos = std::find(fit, feit, *it);
      if (fpos != feit)
        this->_handler.getServer().g_db.getUserTable().removeChannel(
            this->_handler.getServer().g_db.getUserTable().getUser(name), *it);
      else {
        // no result from find() -> the user is not in the channels in args.
        // #define ERR403 "403 :No such channel:"
        // the channel is not exist
        // if (_handler.getServer().g_db.getCorrectChannel(*fpos))
        // the user is not in the channels in args.

        _handler.getMsgBuf(fd).append(ERR442);
        _handler.getMsgBuf(fd).append(*it);
        return;
      }
      ++it;
    }
  }
}

Part::~Part() {}