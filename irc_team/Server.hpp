#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "Channel.hpp"
#include "etc.hpp"
#include <netinet/in.h>

class Server {
public:
  Server(int, std::string);
  ~Server();
  // int joinChannel -> create
  // int deleteChannel
  // void setUser(int);
  int getServerSocket(void);
  struct sockaddr_in &getServerAddr(void);

private:
  std::map<std::string, Channel &> _channel_list;
//   std::map<int, struct s_user_data> _user_list;
  std::string _password;
  int _server_socket;
  struct sockaddr_in _server_addr;
};
// run은 Server에 두는게 맞지 않는가??

#endif /* __SERVER_HPP_ */