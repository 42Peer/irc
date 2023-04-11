#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include <cstdlib>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/_types/_socklen_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include "userStruct.h"

enum{
	FORMATERR = -4, /* wrong msg format or wrong channel format */
	WRONGARG = -3, /* wrong argument number */
	INVAILDCMD = -2,
	EMPTY = -1, /* no cmd or no argu */
	MESSAGE = 0,
	JOIN, /* arg : 1      delimeter : ',' */
	NICK, /* arg : 1 */
	QUIT, /* arg : 0 */
	PRIVMSG, /* arg : 2 */
	KICK, /* arg : 1 */
	PART, /* arg : 0,1    delimeter : ','  */
	LIST, /* arg : 0 */
};

// typedef struct s_user_data {
// 	std::string user_name;
// 	std::string nick_name;
// 	std::vector<std::string> channel_lists;
// } t_user_data;

class Server {
public:
  Server(int, std::string);
  ~Server();
  
  // int joinChannel -> create
  // int deleteChannel
	void setUserInfo(int, std::string, std::string);
	struct s_user_info getUserData(int);

	uintptr_t getServerSocket(void);
	std::string& getServerPassword(void);
	struct sockaddr_in &getServerAddr(void);

  void printErrorMsg(const char *msg) {
  std::cerr << "Error : " << msg << '\n';
  exit(1);
}

  /*
    Commands
  */
  // bool authClient(int);
  // void cmdExecute(int, int, std::vector<std::string>);
  // void cmdJoin(std::string);
  // void cmdNick(s_user_info, std::string);
  // void cmdJoin(void);
  // void cmdPart(void);
  // void cmdQuit(void);
  // void cmdPrivMsg(std::string, std::string);
  // void cmdList(void);
  // void cmdKick(void);
  // void cmdNotice(void);

private:
  // std::map<std::string, ChannelData &> _channel_list;
  std::map<int, struct s_user_info> _user_fd_list;
  std::string _password;
  int _server_socket;
  struct sockaddr_in _server_addr;
};
// run은 Server에 두는게 맞지 않는가??

#endif /* __SERVER_HPP_ */