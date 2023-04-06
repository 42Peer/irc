#include "Handler.hpp"
#include <netinet/in.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/socket.h>

void wrapEvSet(std::vector<struct kevent> &list, int ident, int filter,
               int flag) {
  // We don't know parameter udata when we use
  struct kevent new_event;
  EV_SET(&new_event, ident, filter, flag, 0, 0, 0);
  list.push_back(new_event);
}

Handler::Handler(Server &server_) : _server(server_) {
  _kq = kqueue();
  if (_kq == -1)
    printErrorMsg("kqueue()");
  wrapEvSet(_event_list, _server.getServerSocket(), EVFILT_READ,
            EV_ADD | EV_ENABLE);
}

Handler::~Handler() {}

void Handler::setUserData() {}

void Handler::run(void) {

  int evt;
  while (true) {
    evt = kevent(_kq, &_event_list[0], 8, _monitor, 8, NULL);
    if (evt == -1)
      printErrorMsg("evt()");

    for (int i = 0; i < evt; ++i) {
      if (_monitor[i].flags & EV_EOF)
        std::cout << "Error: Client Disconnect\n";
      else if (_monitor[i].flags & EV_ERROR) {
        std::cerr << "ENABLE\n";
        if (_monitor[i].ident == _server.getServerSocket()) {
          printErrorMsg("Server Socket");
        } else {
          std::cerr << "Client\n";
        }
      } else if (_monitor[i].filter == EVFILT_READ) {
        if (_monitor[i].ident == _server.getServerSocket()) {
          /*******************************************
             we need to move code to server class
            *******************************************/
          socklen_t sock_len = sizeof(sockaddr_in);
          int new_client =
              accept(_server.getServerSocket(),
                     (struct sockaddr *)(&_server.getServerAddr()), &sock_len);
          if (new_client == -1)
            continue;
          setsockopt(new_client, SOL_SOCKET, SO_REUSEADDR, 0, 0);
          if (fcntl(new_client, F_SETFL, O_NONBLOCK) == -1)
            printErrorMsg("fcntl()");
          /*******************************************
              setUserData);
              we decide to make global variable DB in class;
           *******************************************/
        } else {
          // parsing();
          // func name == write ,switchCmd(); <- user , channel,
          // run_cmd();
        }
      }
    }
  }
}