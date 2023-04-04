#include <algorithm>
#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/_types/_socklen_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

void printErrorMsg(const char *);
void initEvent(std::vector<struct kevent> &data, int ident, int filter,
               int flags);


class Channel {
public:
  Channel(int port);
  ~Channel();
  void run();
  void closeAll();
  std::string parsing(std::string&);
private:
  int _port_num; // server
  struct sockaddr_in _server_addr; // server
  int _server_sock; // server 
  std::vector<int> _client_list; //server 
  socklen_t _size; // server
  std::vector<struct kevent> _event_lists;  //
  int _kq;  // handler
  // Server connect;
  // std::map<int, std::string> _socket_addr;
};

Channel::~Channel() {
  closeAll();
}
Channel c(8080);

void Channel::closeAll() {
  for (int i = 0; i < _client_list.size(); ++i) {
    close(_client_list[i]);
  }
  close(_kq);
  close(_server_sock);
}

std::string Channel::parsing(std::string& str) {
  std::string cmd;
  if (str[0] == '/') {
    int idx = str.find(" ");
    cmd = str.substr(1, idx);
    if (cmd == "join")
    {
      std::cout << "join user ";
    }
    return (str.substr(idx + 1, str.size() - idx + 1));
  }
  else {
    return ("");
  }
}

Channel::Channel(int port_) : _port_num(port_) {

  _server_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (_server_sock == -1) {
    printErrorMsg("socket()");
    exit(1);
  }
  memset(&_server_addr, 0, sizeof(_server_addr));
  _server_addr.sin_family = AF_INET;
  _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  _server_addr.sin_port = htons(_port_num);
  _size = sizeof(sockaddr_in);
  if (bind(_server_sock, (sockaddr *)&_server_addr, _size) == -1) {
    printErrorMsg("bind()");
    exit(1);
  }
  if (listen(_server_sock, 10) == -1) {
    printErrorMsg("listen()");
    exit(1);
  }
}

void handler(int no) { (void)no; c.closeAll(); }

void Channel::run() {
  // int kq;
  initEvent(_event_lists, _server_sock, EVFILT_READ, EV_ADD | EV_ENABLE);
  struct kevent monitor[8];
  std::cout << "run";

  if ((_kq = kqueue()) == -1) {
    // std::cerr << "kqueue() error " << std::endl;
    printErrorMsg("kqueue()");
    // run();
  }
  int evt;
  signal(SIGINT, handler);
  while (true) {
    evt = kevent(_kq, &_event_lists[0], _event_lists.size(), monitor, 8, NULL);
    if (evt == -1) {
      printErrorMsg("kevent()");
      return;
    }
    _event_lists.clear();

    for (int i = 0; i < evt; ++i) {
      std::vector<int>::iterator iter;
      std::cout << "Flags : " << monitor[i].flags << " " << std::endl;
      if (monitor[i].flags & EV_EOF) {
        std::cout << "Client : disconnected : " << monitor[i].ident << '\n';
        close(monitor[i].ident);
      }
      if (monitor[i].flags & EV_ERROR) {
        printErrorMsg("ENABLE");
        if (monitor[i].ident == _server_sock)
          printErrorMsg("Server Sock Error");
        else {
          // "\n"; close(monitor[i].ident);
          printErrorMsg("Client Error");
        }
      } else if (monitor[i].filter == EVFILT_READ) {
        // std::cout << _server_sock << " " << monitor[i].ident << "\n";
        if (monitor[i].ident == _server_sock) {
          int clnts;
          clnts = accept(_server_sock, (sockaddr *)&_server_addr, &_size);
          if (clnts == -1) {
            printErrorMsg("accept()");
          }
          fcntl(clnts, F_SETFL, O_NONBLOCK);
          std::cout << "new client added : " << clnts << "\n";
          initEvent(_event_lists, clnts, EVFILT_READ, EV_ADD | EV_ENABLE);
          // initEvent(_event_lists, clnts, EVFILT_WRITE, EV_ADD | EV_ENABLE);
          _client_list.push_back(clnts);
        } else if ((iter = std::find(_client_list.begin(), _client_list.end(),
                                     monitor[i].ident)) != _client_list.end()) {
          std::string ret;
          std::string who_am_i;
          char buf[1024];
          int r;
          who_am_i = "Client[";
          who_am_i += std::to_string(monitor[i].ident);
          who_am_i += "] : ";
          while ((r = read(monitor[i].ident, buf, 1024)) > 0) {
            buf[r] = 0;
            ret += buf;
          }
          
          if ((ret = parsing(ret)) == "") {
            std::cout << "cannot found cmd"<<std::endl;
            for (int j = 0; j < _client_list.size(); ++j)
            {
             send(_client_list[j], who_am_i.c_str(), who_am_i.size() + 1, 1);
             send(_client_list[j], ret.c_str(), ret.size(), 1);
             send(_client_list[j], "\n", 2, 1);
            }  
            } else {
              std::cout << ret << std::endl;
              std::cout << "found cmd" << std::endl;
            }
        }
      }
    }
  }
}

void initEvent(std::vector<struct kevent> &data, int ident, int filter,
               int flags) {
  struct kevent tmp;

  EV_SET(&tmp, ident, filter, flags, 0, 0, 0);
  data.push_back(tmp);
}

void printErrorMsg(const char *msg) { std::cerr << "Error : " << msg << '\n'; }

int main(void) {
  // Channel c(8080);
  c.run();
  return 0;
}