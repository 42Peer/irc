#include "Server.hpp"

Server::Server(int port_, std::string password_, Channel& Ref) : _password(password_), _channel(Ref)
{
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
	std::map<int, std::string >::iterator it = _fd_name_map.begin();
	while(it != _fd_name_map.end())
	{
		close(it->first);
		++it;
	}
  // To destroy channel lists, we need to discuss later
  // _channel_list.clear();
	close(_server_socket);
}

uintptr_t Server::getServerSocket(void) { return _server_socket; }

Channel&	Server::getChannelRef(void)
{
	return (this->_channel);
}

std::string	Server::getUserName(int fd)
{
	return (_fd_name_map[fd]);
}

// int		Server::getUserFd(std::string name)
// {
// 	return (_name_fd_map[name]);
// }

void	Server::setMapData(int fd, std::string name)
{
	_fd_name_map[fd] = name;
	// _name_fd_map[name] = fd;
}

void	Server::removeMapData(int fd) {
	_fd_name_map[fd].erase();
}

struct sockaddr_in &Server::getServerAddr(void) { return _server_addr; }