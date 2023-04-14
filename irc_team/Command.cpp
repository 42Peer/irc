#include "Command.hpp"
#include "Db.hpp"
#include "userStruct.h"
#include <algorithm>
#include <vector>

#define ERR461 "461 :Not enough parameters:\n"
#define ERR476 "476 :Invalid channel name:\n"
#define ERR401 "401 :No such nick or channel:\n"
#define ERR404 "404 :Cannot send to channel:\n"
#define ERR433 "433 :Nickname is already in use:\n"
#define ERR407 "407 :Duplicate recipients. No message delivered:\n"
#define ERR412 "412 :No text to send:\n"
#define ERR403 "403 :No such channel:\n"
#define ERR442 "442 :You're not on that channel:\n"
#define ERR462 "462 :You may not register:\n"
#define ERR432 "432 :Erroneus nickname\n"
#define ERR482 "482 :You're not channel operator\n"

#define MSGNOTICE " NOTICE :"
#define MSGJOIN " JOIN :"
#define MSGNICK " NICK :"
#define MSGQUIT " QUIT :"
#define MSGPRIVMSG " PRIVMSG :"
#define MSGKICK " NICK :"
#define MSGPART " PART :"

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

bool existInVector(std::vector<std::string> &vec, std::string value) {
  std::vector<std::string>::iterator it = vec.begin();
  for (; it != vec.end(); ++it) {
    if (*it == value)
      return (true);
  }
  return (false);
}

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
  std::string buf("");
  //	404 ERR_CANNOTSENDTOCHAN "<channel name> :Cannot send to channel"
  if (args.front()[0] == '#') {
    buf.append(ERR404);
    // buf.append(args.front());
    // send(fd, buf.c_str(), buf.size(), 0);
    this->_handler.getServer().setFdMessage(fd, buf);
  }
  // 401 ERR_NOSUCHNICK " <nickname>: No such (user1)nick/channel"
  else if (this->_handler.getServer().g_db.getUserTable().isExist(
               args.front()) == false) {
    buf.append(ERR401);
    // buf.append(args.front());
    // send(fd, buf.c_str(), buf.size(), 0);
    this->_handler.getServer().setFdMessage(fd, buf);
  }
  // nickname NOTICE 대상닉네임 :text
  else {
    buf.append(":" + this->_handler.getServer().getUserName(fd) + MSGNOTICE + args.front() + " :" + args.back() + "\n");
    int receiver =
        this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd;
    // send(
    //     this->_handler.getServer().g_db.getUserTable().getUser(args.front()).fd,
    //     buf.c_str(), buf.size(), 0);
    this->_handler.getServer().setFdMessage(receiver, buf);
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

  // vector<string>  [#123] [#4555] [#1]

  std::vector<std::string>::iterator it_channel_name = args.begin();
  std::vector<std::string>::iterator eit = args.end();
  std::string nick_name = this->_handler.getServer().getUserName(fd);
  std::string buf("");
  for (int i = 0; it_channel_name != eit; ++it_channel_name, ++i) {
    // size_t pos = it_channel_name->find('#');
    // if (pos != std::string::npos) {
    if (args[i][0] != '#') {
      // 476 :Invalid channel name: 123
      buf.append(ERR476);
      //   buf.append(*it_channel_name);
      //   send(fd, buf.c_str(), buf.size(), 0);
      this->_handler.getServer().setFdMessage(fd, buf);
      return;
    } else {
//      std::string nick_name = this->_handler.getServer().getUserName(fd);
      // chanel, server map 변수 업데이트 해줘야함.
      this->_handler.getServer().g_db.addChannelUser(
          this->_handler.getServer().g_db.getUserTable().getUser(nick_name),
          *it_channel_name);
      // jujeon JOIN :#999
      buf.append(":" + nick_name + MSGJOIN + *it_channel_name + "\n");
      //   send(fd, buf.c_str(), buf.size(), 0);
      this->_handler.getServer().setFdMessage(fd, buf);
      // 채널에 속한 유저들에게 똑같은 메세지를 send() 해줘야함
      std::vector<std::string> userList =
          this->_handler.getServer()
              .g_db.getCorrectChannel(*it_channel_name)
              .getUserList();
      std::vector<std::string>::iterator it_all_user_list = userList.begin();
      std::vector<std::string>::iterator eit_all_user_list = userList.end();

      for (; it_all_user_list != eit_all_user_list; ++it_all_user_list) {
        int receiver = this->_handler.getServer().g_db.getUserTable().getUser(*it_all_user_list).fd;
        this->_handler.getServer().setFdMessage(receiver, buf);
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
                  (1-2) 존재하지 않으면 서버 map 에 닉네임 변경해주기
                          (1-2-1) 그리고, db에 닉네임도 변경해주기
          (2) NICK :hello 메세지 띄워주기
                        (2-1) 현재 있는 채널을 찾고, 같은 채널에 있는 사람에게
     알려주기 root NICK :jujeon

          error case
                  :irc.local 433 jujeon root_ :Nickname is already in use.
                  433     ERR_NICKNAMEINUSE "<nick> :Nickname is already in use"

  */
  // std::string old_name = this->_handler.getServer().getUserName(fd);
  // std::string new_name = args.front();
  // struct s_user_info old_user_info =
  // this->_handler.getServer().g_db.getUserTable().getUser(old_name); struct
  //   s_user_info new_user_info;

  //   new_user_info.nick = new_name;
  //   new_user_info.name = old_user_info.name;
  //   new_user_info.fd = old_user_info.fd;
  //   new_user_info.channel_list = old_user_info.channel_list;

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

  std::string buf("");
  std::string new_nick = args[0];
  if (new_nick[0] == '#') {
    buf.append(ERR432);
    this->_handler.getServer().setFdMessage(fd, buf);
    return;
  }
  if (this->_handler.getServer().g_db.getUserTable().isExist(new_nick)) {
    buf.append(ERR433);
    this->_handler.getServer().setFdMessage(fd, buf);
    return;
  } else {
    std::string old_name = this->_handler.getServer().getUserName(fd);
    s_user_info old_user_info =
        this->_handler.getServer().g_db.getUserTable().getUser(old_name);
    s_user_info new_user_info;
    new_user_info.nick = new_nick;
    new_user_info.name = old_user_info.name;
    new_user_info.fd = old_user_info.fd;
    new_user_info.channel_list = old_user_info.channel_list;

    this->_handler.getServer().setMapData(fd, args.front());
    this->_handler.getServer().g_db.updateUser(old_user_info, new_user_info);

    buf.append(":" + this->_handler.getServer().getUserName(fd) + MSGNICK + new_nick + "\n");
    if (this->_handler.getFdflags().find(fd) !=
        this->_handler.getFdflags().end()) {
		this->_handler.setFdFlags(fd, 2);
		this->_handler.getServer().g_db.getUserTable().getUser(new_nick).fd = fd;
	}
    // send(fd, buf.c_str(), buf.size(), 0);

    // 같은 채널 사람들에게 보여주기
    std::string current_channel =
        this->_handler.getServer().g_db.getUserTable().getChannelList(
            new_user_info);
    std::vector<std::string> users_in_current_channel =
        this->_handler.getServer()
            .g_db.getCorrectChannel(current_channel)
            .getUserList();
    std::vector<std::string>::iterator it_users =
        users_in_current_channel.begin();
    std::vector<std::string>::iterator eit_users =
        users_in_current_channel.end();
    for (; it_users < eit_users; ++it_users) {
      int receiver =
          this->_handler.getServer().g_db.getUserTable().getUser(*it_users).fd;
      //   send(receiver, buf.c_str(), buf.size(), 0);
      this->_handler.getServer().setFdMessage(receiver, buf);
    }
  }
}

Nick::~Nick() {}

void Quit::run(int fd, std::vector<std::string> args) {
  (void)args;
  std::string usr_name = this->_handler.getServer().getUserName(fd);
  struct s_user_info usr_name_info =
      this->_handler.getServer().g_db.getUserTable().getUser(usr_name);
  //   std::vector<std::string>::iterator it =
  //   usr_name_info.channel_list.begin(); while (it !=
  //   usr_name_info.channel_list.end()) {
  //     this->_handler.getServer().getChannelRef().removeUserFromChannel(*it,
  //     usr_name);
  //     ++it;
  //   }
  this->_handler.getServer().g_db.removeUser(usr_name_info);
  this->_handler.getServer().removeMapData(fd);
  close(fd);
}

Quit::~Quit() {}

std::vector<bool> duplicated_args(std::vector<std::string>& args) {
  std::map<std::string, int> check;
  std::vector<bool> ret_arg;
  ret_arg.resize(args.size() - 1);
  int cnt = 0;
  for (size_t i = 0; i < args.size() - 1; ++i) {
    int tmp = check[args[i]];
    if (tmp != 0) {
      ret_arg[i] = true;
    } else {
      ret_arg[i] = false;
    }
    check[args[i]] = tmp + 1;
  }
  return (ret_arg);
}
// handler
void Privmsg::run(int fd, std::vector<std::string> args) {
  std::vector<bool> res_args = duplicated_args(args);
  std::vector<std::string> users;
  UserData user = this->_handler.getServer().g_db.getUserTable();
  std::string msg;
  msg = ":" + this->_handler.getServer().getUserName(fd) + " PRIVMSG ";
  for (size_t i = 0; i < args.size() - 1; ++i) {
    if (!user.isExist(args[i])) {
      if (args[i][0] == '#') {
        ChannelData chn =
            this->_handler.getServer().g_db.getCorrectChannel(args[i]);
        if (!chn.findUser(this->_handler.getServer().getUserName(fd))){
          this->_handler.getServer().setFdMessage(fd, ERR442);
          continue;
        }
        if (res_args[i] == true) {
          this->_handler.getServer().setFdMessage(fd, ERR407);
          continue;
        }
        std::vector<std::string> user_lists = chn.getUserList();
        for (size_t j = 0; j < user_lists.size(); ++j) {
          this->_handler.getServer().setFdMessage(
              user.getUser(user_lists[j]).fd,
              msg + user_lists[j] + " :" + args.back() + ";\n\t" +
                  " Message from " +
                  this->_handler.getServer().getUserName(fd) + " to " +
                  user_lists[j] + "\n");
        }
        continue;
      }
      this->_handler.getServer().setFdMessage(fd, ERR401);
      continue;
    }
    if (res_args[i] == true) {
      this->_handler.getServer().setFdMessage(fd, ERR407);
      continue;
    }
    struct s_user_info cur_user = user.getUser(args[i]);
    this->_handler.getServer().setFdMessage(
        cur_user.fd, msg + cur_user.nick + " :" + args.back() + ";" +
                         " Message from " +
                         this->_handler.getServer().getUserName(fd) + " to " +
                         cur_user.nick + "\n");
  }
}

Privmsg::~Privmsg() {}

void Kick::run(int fd, std::vector<std::string> args) {
  //   std::string name = this->_handler.getServer().getUserName(fd);
  //   if (this->_handler.getServer()
  //           .g_db.getUserTable()
  //           .getUser(name)
  //           .channel_list.size() <= 0)
  //     return; /* error msg */
  //   std::string kicked_name = args.front();
  //   std::string current_channel = this->_handler.getServer()
  //                                     .g_db.getUserTable()
  //                                     .getUser(name)
  //                                     .channel_list.back();
  //   if (!this->_handler.getServer().g_db.getUserTable().isExist(kicked_name))
  //     return; /* error msg */
  //   else if (!(this->_handler.getServer().getChannelRef().isExistInChannel(
  //                current_channel, kicked_name)))
  //     return; /* error msg */
  //   if (this->_handler.getServer()
  //           .g_db
  //           .getCorrectChannel(this->_handler.getServer()
  //                                  .g_db.getUserTable()
  //                                  .getUser(name)
  //                                  .channel_list.back())
  //           .getPrivileges(name) != -1) {
  //     this->_handler.getServer().g_db.getUserTable().removeChannel(
  //         this->_handler.getServer().g_db.getUserTable().getUser(kicked_name),
  //         current_channel);
  //     this->_handler.getServer().getChannelRef().removeUserFromChannel(
  //         current_channel, kicked_name);
  // send(this->_handler.getServer().g_db.getUserTable().getUser(kicked_name).fd,)
  // /* send kicked msg */
  //   } else
  //     return; /* error msg */

  // kick <channel> <user> :
  // 	:root_!root@127.0.0.1 KICK #123 :kick #123 hello

  // kick:
  // 	ERR_NEEDMOREPARAMS: 파라미터 불일치
  // 	- 461 <command> :Not enough parmeters
  // 	ERR_NOSUCHCHANNEL: 채널 존재하지 않음
  // 	- 403 <channel name> :No such channel
  // 	ERR_BADCHANMASK: 잘못된 채널 마스크
  // 	- <server_name> 476 <nickname> ERR_BADCHANMASK #channel :Bad Channel
  // Mask 	ERR_CHANOPRIVSNEEDED: 채널 관리자 권한 없음
  // 	- 482 <channel> :You're not channel operator
  // 	ERR_NOTONCHANNEL: 채널에 속하지 않음
  //   - 442 <channel> :You're not on that channel

  /*
          kick <channel> <name> [reason]:
                  KICK #123 dllee :reason
    x(0) 파라미터 개수 2개 ~ 3개인지 확인
      x(0-0) 파라미터 개수 부족시 에러 => 461 ERR_NEEDMOREPARAMS
      x(0-1) 파라미터 개수 충족시 다음 단계
    (1) channel에 #이 존재하는지 체크
      (1-0) #이 존재하지 않으면 에러 => 476 ERR_BADCHANMASK ? 401
      (1-1) db에 해당 채널이 존재하는지 체크
        (1-1-0) db에 해당 채널이 없는 경우 => 442 ERR_NOTONCHANNEL
        (1-1-1) db에 해당 채널 존재하는 경우 다음 단계
    (2) name이 존재하는지 체크
      (2-0) 해당 채널에 name이 db에 존재하지 않으면 에러 => 401/441 :No such
    nick (2-1) 본인이 해당 채널에 있는지 확인 (2-1-0) name이 존재할 경우 해당
    채널에서 권한 확인 후 name 추방 (2-1-1) name이 존재할 경우 해당 채널에 권한
    없는 경우 에러 => 482 (3) reason이 존재하는지 체크 (3-1) 없는 경우 전송
      (3-2) reason을 추가하여 전송
  */
  std::string tmp_msg = "";

  if (args[0][0] != '#') {
    _handler.getServer().setFdMessage(fd, ERR401); // ERR 확인!!
    return;
  } else {
    bool check = _handler.getServer().g_db.addChannel(args[0]);
    _handler.getServer().g_db.channelClear(args[0]);
    if (check) { // channel 존재여부 확인
      _handler.getServer().setFdMessage(fd, ERR442);
      return;
    }
  }

  ChannelData tmp_channel =
      _handler.getServer().g_db.getCorrectChannel(args[1]);
  if (!_handler.getServer().g_db.isExist(args[1])) {
    _handler.getServer().setFdMessage(fd, ERR401);
    return;
  } else {
    if (!tmp_channel.findUser(args[1])) {
      _handler.getServer().setFdMessage(fd, ERR482);
      return;
    }
  }

  std::string tmp_name = _handler.getServer().getUserName(fd);
  if (!tmp_channel.findUser(tmp_name)) {
    _handler.getServer().setFdMessage(fd, ERR442);
    return;
  }

  if (tmp_channel.getPrivileges(tmp_name) > 0) {
    _handler.getServer().setFdMessage(fd, ERR482);
  } else {
    tmp_channel.removeData(args[1]);
    tmp_msg = "KICK " + args[0] + " " + args[1];
    if (args.size() == 3) {
      tmp_msg += ":";
      tmp_msg += args[2];
      tmp_msg += "\n";
      _handler.getServer().setFdMessage(fd, tmp_msg);
    } else {
      tmp_msg += "\n";
      _handler.getServer().setFdMessage(fd, tmp_msg);
    }
  }
}

Kick::~Kick() {}

void Part::run(int fd, std::vector<std::string> args) {

  std::string name = this->_handler.getServer().getUserName(fd);

  // 442 ERR_NOTONCHANNEL "<channel> :You're not on that channel"
  if (this->_handler.getServer()
          .g_db.getUserTable()
          .getUser(name)
          .channel_list.size() == 0) {
    _handler.getServer().setFdMessage(fd, ERR442);
    return;
  } else if (!args.size()) {
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
        // ERR403 "403 :No such channel:"
        if (_handler.getServer().g_db.getCorrectChannel(*fpos).isEmpty()) {
          _handler.getServer().setFdMessage(fd, ERR403);
          _handler.getServer().setFdMessage(fd, *fpos);
          _handler.getServer().g_db.channelClear(*fpos);
        } else {
          // the user is not in the channels in args.
          _handler.getServer().setFdMessage(fd, ERR442);
          _handler.getServer().setFdMessage(fd, *it);
        }
        return;
      }
      ++it;
    }
  }
}

Part::~Part() {}

void User::run(int fd, std::vector<std::string> args) {
  std::string name = this->_handler.getServer().getUserName(fd);
  struct s_user_info info =
      this->_handler.getServer().g_db.getUserTable().getUser(name);
  info.name = args[0];
  info.real = args[3];
  if (this->_handler.getFdflags().find(fd) != this->_handler.getFdflags().end())
    this->_handler.getFdflags().erase(fd);
}

void Pass::run(int fd, std::vector<std::string> args) {
  if (this->_handler.getServer().getServerPassword() == args[0]) {
    if (this->_handler.getFdflags().find(fd) !=
        this->_handler.getFdflags().end())
      this->_handler.setFdFlags(fd, 1);
  }
}