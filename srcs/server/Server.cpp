#include "Server.hpp"

Server::Server(int port_, std::string password_) : _password(password_)
{
	if ((_server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		std::cerr << "socket err\n";
		exit(1);
	}
	setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, 0, 0);

	memset(&_server_addr, 0, sizeof(sockaddr_in));
	_server_addr.sin_family = AF_INET;
	_server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	_server_addr.sin_port = htons(port_);

	if (bind(_server_socket, (struct sockaddr *)(&_server_addr), sizeof(sockaddr)) == -1)
	{
		std::cerr << "bind err\n";
		exit(1);
	}

	if (listen(_server_socket, 42) == -1)
	{
		std::cerr << "listen err\n";
		exit(1);
	}
}

Server::~Server()
{
	std::map<int, std::string>::iterator it = _fd_name_map.begin();
	while (it != _fd_name_map.end())
	{
		close(it->first);
		++it;
	}
	close(_server_socket);
}

uintptr_t Server::getServerSocket(void) { return _server_socket; }

std::string Server::getUserName(int fd)
{
	if (_fd_name_map.find(fd) == _fd_name_map.end())
		return ("");
	return (_fd_name_map[fd]);
}

void Server::setMapData(int fd, std::string name)
{
	_fd_name_map[fd] = name;
}

void Server::removeMapData(int fd)
{
	_fd_name_map[fd].erase();
}

struct sockaddr_in &Server::getServerAddr(void) { return _server_addr; }

std::string &Server::getServerPassword(void) { return _password; }

void Server::setFdMessage(int fd, std::string data)
{
	_fd_message[fd] += data;
}

std::string& Server::getFdMessage(int fd)
{
	return (_fd_message[fd]);
}

void	Server::removeFdMessage(int fd)
{
	_fd_message[fd].erase();
}

void Server::setFdFlags(int fd)
{
	/*
		0 - check PASS
		1 - check NICK
		2 - check USER
		3 - check AllInitStatus
		3 - check PART
	*/
	std::vector<bool> temp;
	temp.push_back(false);
	temp.push_back(false);
	temp.push_back(false);
	temp.push_back(false);
	temp.push_back(false);
	_fd_flags[fd] = temp;
}

void Server::setFdFlagsOn(int fd, int i)
{
	_fd_flags[fd][i] = true;
}

bool Server::getFdFlagsInitStatus(int fd)
{
	if (_fd_flags[fd][0] && _fd_flags[fd][1] && _fd_flags[fd][2] && _fd_flags[fd][3])
		return (true);
	return (false);
}

bool Server::getFdFlagsStatus(int fd, int i)
{
	return (_fd_flags[fd][i]);
}

bool Server::checkGreetingMessage(int fd)
{
	if (_fd_flags[fd][0] && _fd_flags[fd][1] && _fd_flags[fd][2] && !_fd_flags[fd][3])
		return (true);
	return (false);
}

void	Server::removeFdFlags(int fd)
{
	_fd_flags.erase(fd);
}
