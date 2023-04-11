#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "baseHeader.h"
#include "userStruct.h"
#include "Db.hpp"
#include "Channel.hpp"

extern Db g_db;

class Channel;
class Server {
	public:
		Server(int, std::string, Channel&);
		~Server();

		uintptr_t getServerSocket(void);
		std::string& getServerPassword(void);
		struct sockaddr_in &getServerAddr(void);
		Channel&	getChannelRef(void);

		void	setMapData(int, std::string);

		std::string	getUserName(int);
		void	removeMapData(int);
		// int	getUserFd(std::string);

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
		std::map<int, std::string> _fd_name_map;
		// std::map<std::string, int> _name_fd_map;

		std::string _password;
		int _server_socket;
		struct sockaddr_in _server_addr;
		Channel& _channel;
};

void printErrorMsg(const char *msg) {
	std::cerr << "Error : " << msg << '\n';
	exit(1);
}
#endif /* __SERVER_HPP_ */