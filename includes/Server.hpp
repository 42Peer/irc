#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "baseHeader.hpp"
#include "userStruct.hpp"
#include "Db.hpp"

// class Channel;
class Server {
	public:
		Server(int, std::string);
		~Server();

		uintptr_t getServerSocket(void);
		std::string& getServerPassword(void);
		struct sockaddr_in &getServerAddr(void);

		void	setMapData(int, std::string);

		std::string	getUserName(int);
		void	removeMapData(int);
		void	setFdMessage(int, std::string);
		std::string& getFdMessage(int);
		Db	g_db;

	private:
		std::map<int, std::string> _fd_name_map;
        std::map<int, std::string> _fd_message;

		std::string _password;
		int _server_socket;
		struct sockaddr_in _server_addr;
};

#endif /* __SERVER_HPP_ */