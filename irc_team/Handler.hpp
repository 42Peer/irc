#ifndef __HANDLER_HPP_
#define __HANDLER_HPP_

#include "Server.hpp"
#include "etc.hpp"

class Handler {
public:
  Handler(Server &);
  ~Handler();
	void run(void);
private:
  int _kq;
  std::vector<struct kevent> _event_list;
  struct kevent _monitor[8];
  Server &_server;

  void callPaser(std::string);
};


#endif /* __HANDLER_HPP_ */
