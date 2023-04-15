#ifndef __HANDLER_HPP_
#define __HANDLER_HPP_

#include "Server.hpp"

class Handler {
	public:
		Handler(Server&);
		~Handler();
		void run(void);
		Server& getServer(void);
		void    setFdFlags(int, int);
		std::map<int, int>& getFdflags(void);
private:
	int _kq;
	std::vector<struct kevent> _event_list;
	struct kevent _monitor[8];
	std::map<int, std::pair<std::string, std::string> > _msgMap;
	std::map<int, int> _fd_flags;
	Server& _server;

	void callPaser(std::string);
	void figureCommand(int, std::pair<int, std::vector<std::string> >&);
	bool servReceive(int);
	void makeProtocol(int);
};


#endif /* __HANDLER_HPP_ */