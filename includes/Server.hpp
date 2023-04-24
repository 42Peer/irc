#ifndef __SERVER_HPP_
#define __SERVER_HPP_

#include "baseHeader.hpp"
#include "userStruct.hpp"
#include "Db.hpp"

class Server {
	public:
		Server(int, std::string);
		~Server();

		uintptr_t getServerSocket(void);
		std::string& getServerPassword(void);
		struct sockaddr_in &getServerAddr(void);

		void	setMapData(int, std::string);
		void	removeMapData(int);
		std::string	getUserName(int);

		void	setFdMessage(int, std::string);
		std::string& getFdMessage(int);
		void	updateFdMessage(int, size_t);
		void	removeFdMessage(int);

		void	setFdFlags(int);
		void	setFdFlagsOn(int, int);
		bool	getFdFlagsInitStatus(int);
		bool	getFdFlagsStatus(int, int);
		bool	checkGreetingMessage(int);
		void	removeFdFlags(int);

		void	setFdTempInfo(int, struct s_user_info&);
		bool	findFdTempInfo(int);
		void	removeFdTempInfo(int);
		std::string	getFdTempInfo(int, int);
		Db	g_db;

	private:
		std::map<int, std::string> _fd_name_map;
		std::map<int, std::string> _fd_message;
		std::map<int, std::vector<bool> > _fd_flags;
		std::map<int, struct s_user_info> _fd_temp_info;

		std::string _password;
		int _server_socket;
		struct sockaddr_in _server_addr;
};

#endif /* __SERVER_HPP_ */