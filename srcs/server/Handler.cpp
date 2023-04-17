#include "Handler.hpp"
#include "Command.hpp"
#include "Parsing.hpp"
#include "userStruct.hpp"
#include "Message.hpp"

int findCrln(std::string &line) {
    for (size_t i = 0; i < line.size(); ++i) {
        if ((line[i] == '\r' && line[i + 1] == '\n') || line[i] == '\n')
            return (i);
    }
    return (-1);
}

void printErrorMsg(const char *msg) {
	std::cerr << "Error : " << msg << '\n';
	exit(1);
}
void wrapEvSet(std::vector<struct kevent> &list, int ident, int filter, int flag) {
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


void Handler::run(void) {
	std::map<int, std::string> tmp_data;

	int evt;
	while (true) {
		evt = kevent(_kq, &_event_list[0], _event_list.size(), _monitor, 8, NULL);
		if (evt == -1)
			printErrorMsg("evt()");
		_event_list.clear();

		for (int i = 0; i < evt; ++i) {
			if (_monitor[i].flags & EV_EOF) {
				close(_monitor[i].ident);
			}
            else if (_monitor[i].flags & EV_ERROR) {
				// std::cerr << "ENABLE\n";
				if (_monitor[i].ident == _server.getServerSocket())
					printErrorMsg("Server error");
				else
					;
				//   std::cerr << "Client error\n";
			} else if (_monitor[i].filter == EVFILT_READ) {
				if (_monitor[i].ident == _server.getServerSocket()) {
					socklen_t sock_len = sizeof(sockaddr_in);
					int new_client = accept(_server.getServerSocket(), (struct sockaddr *)(&_server.getServerAddr()), &sock_len);
					if (new_client == -1) {
						continue;
					}
					send(this->getServer().getServerSocket(), "#new\n", 6, 0);
					setsockopt(new_client, SOL_SOCKET, SO_REUSEADDR, 0, 0);
					if (fcntl(new_client, F_SETFL, O_NONBLOCK) == -1)
						printErrorMsg("fcntl()");

					wrapEvSet(_event_list, new_client, EVFILT_READ, EV_ADD | EV_ENABLE);
					wrapEvSet(_event_list, new_client, EVFILT_WRITE, EV_ADD | EV_ENABLE);

					_fd_flags[new_client] = 0;
				}
                else if (servReceive(_monitor[i].ident)) {
                    int idx;
                    while ((idx = findCrln(_msgMap[_monitor[i].ident].first)) != -1) {
                        std::string test =_msgMap[_monitor[i].ident].first.substr(0, idx);
                        std::cout << "recv data : " <<  test << "\n";
                        std::pair<int, std::vector<std::string> > parsed_data = parseData(test);
                        _msgMap[_monitor[i].ident].first.erase(0, idx + 2 - (_msgMap[_monitor[i].ident].first[i] == '\n'));
                        figureCommand(_monitor[i].ident, parsed_data);
                    }
				}
            } else if (_monitor[i].filter == EVFILT_WRITE) {
                std::string fd_data = this->getServer().getFdMessage(_monitor[i].ident);
                if (fd_data == "")
                    continue ;
                std::cout << fd_data << "\n";
                send(_monitor[i].ident, fd_data.c_str(), fd_data.size(), 0);
                this->getServer().getFdMessage(_monitor[i].ident).clear();
			}
		}
	}
}

bool Handler::servReceive(int fd) {
	std::string ret;
	char buf[1024];
	int buf_len;

	memset(buf, 0, sizeof(buf));
	buf_len = recv(fd, (void *)buf, 1024, MSG_DONTWAIT);
	if (buf_len == 0 || buf[0] == '\n')
		return (false);
	buf[buf_len] = '\0';

	_msgMap[fd].first += std::string(buf);

	return (true);
}

void Handler::makeProtocol(int fd) {
	size_t delimiter;
	while ((delimiter = std::min(_msgMap[fd].first.find('\r'), _msgMap[fd].first.find('\n'))) != std::string::npos) {
		std::string data = _msgMap[fd].first.substr(0, delimiter);
		_msgMap[fd].first.erase(0, delimiter + 1);
		_msgMap[fd].second = data;
        if (data == "")
            break ;
		data.clear();
	}
}

void Handler::figureCommand(int fd, std::pair<int, std::vector<std::string> > &data) {
	if (_fd_flags.find(fd) != _fd_flags.end()) {
		Command *cmd = NULL;
		if (data.first == PASS) {
			cmd = new Pass(*this);
			cmd->run(fd, data.second);
			delete cmd;
		} else if (data.first == NICK && _fd_flags[fd] > 0) {
			cmd = new Nick(*this);
			cmd->run(fd, data.second);
			delete cmd;
		} else if (data.first == USER && _fd_flags[fd] > 1) {
			cmd = new User(*this);
			cmd->run(fd, data.second);
			delete cmd;
		} else if (data.first == CAP)
			;
		else
		{
			if (data.first == INVAILDCMD)
				this->getServer().setFdMessage(fd, ERR421 + data.second[0] + "\n");
			else if (data.first == WRONGARG)
				this->getServer().setFdMessage(fd, ERR461);
			else
				this->getServer().setFdMessage(fd, ERR451);
		}
	} else {
		Command *cmd = NULL;
		switch (data.first) {
		case JOIN:
			cmd = new Join(*this);
			break;
		case NICK:
			cmd = new Nick(*this);
			break;
		case QUIT:
			cmd = new Quit(*this);
			break;
		case PRIVMSG:
			cmd = new Privmsg(*this);
			break;
		case KICK:
			cmd = new Kick(*this);
			break;
		case PART:
			cmd = new Part(*this);
			break;
		case NOTICE:
			cmd = new Notice(*this);
			break;
		case USER:
			cmd = new User(*this);
			break;
		case PASS:
			cmd = new Pass(*this);
			break;
		case PING:
			cmd = new Ping(*this);
			break;
		default:
			{
				if (data.first == WRONGARG)
					this->getServer().setFdMessage(fd, ERR461);
				else if (data.first == CAP)
					return ;
				else
					this->getServer().setFdMessage(fd, ERR421 + data.second[0] + "\n");
				return;
			}
		}
		cmd->run(fd, data.second);
		delete cmd;
	}
}

std::map<int, int> &Handler::getFdflags(void) { return (_fd_flags); }

Server &Handler::getServer(void) { return (_server); }

void Handler::setFdFlags(int fd, int flags) {
	if (this->_fd_flags.find(fd) != _fd_flags.end())
		_fd_flags[fd] = flags;
}