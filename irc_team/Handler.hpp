#ifndef __HANDLER_HPP_
#define __HANDLER_HPP_

#include "Server.hpp"

class Handler {
	public:
		Handler(Server&);
		~Handler();
		void run(void);
		Server& getServer(void);
private:
  int _kq;
  std::vector<struct kevent> _event_list;
  struct kevent _monitor[8];
  std::map<int, std::string> _msgMap;
  Server& _server;

  void callPaser(std::string);
};


#endif /* __HANDLER_HPP_ */