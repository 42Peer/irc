#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "baseHeader.h"
#include "userStruct.h"
#include "Db.hpp"
#include "Channel.hpp"

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
		Db	g_db;
		// int	getUserFd(std::string);

	private:
		std::map<int, std::string> _fd_name_map;
        std::map<int, std::string> _fd_message;
		// std::map<std::string, int> _name_fd_map;

		std::string _password;
		int _server_socket;
		struct sockaddr_in _server_addr;
		Channel& _channel;
};

#endif /* __SERVER_HPP_ */