#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "baseHeader.h"
#include "userStruct.h"
#include "Db.hpp"

Db g_db;

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

#endif /* __SERVER_HPP_ */