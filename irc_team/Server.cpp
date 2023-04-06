#include "Server.hpp"
#include <netinet/in.h>
#include <sys/socket.h>

Server::Server(int port_, std::string password_) : _password(password_) {
    _server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (_server_socket == -1)
    printErrorMsg("Socket()");
  setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, 0, 0);
  memset(&_server_addr, 0, sizeof(sockaddr_in));
  _server_addr.sin_family = AF_INET;
  _server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  _server_addr.sin_port = htons(port_);
  if (bind(_server_socket, (struct sockaddr *)(&_server_addr),
           sizeof(sockaddr)) == -1)
    printErrorMsg("Bind()");
  if (listen(_server_socket, 42) == -1)
    printErrorMsg("Listen()");
}

Server::~Server() {
  std::map<int, struct s_user_data>::iterator iter;
  for (iter = _user_list.begin(); iter != _user_list.end(); ++iter) {
    close(iter->first);
  }
  // To destroy channel lists, we need to discuss later
  // _channel_list.clear();
  close(_server_socket);
}

int Server::getServerSocket(void) { return _server_socket; }

void Server::setUser(int fd) {
  struct s_user_data new_user;
  new_user.socket_id = fd;
  _user_list[fd] = new_user;
}

struct sockaddr_in &Server::getServerAddr(void) { return _server_addr; }