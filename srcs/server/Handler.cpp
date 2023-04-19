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
				this->getServer().removeFdFlags(_monitor[i].ident);
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
					send(this->getServer().getServerSocket(), "#new\r\n", 7, 0);
					setsockopt(new_client, SOL_SOCKET, SO_REUSEADDR, 0, 0);
					if (fcntl(new_client, F_SETFL, O_NONBLOCK) == -1)
						printErrorMsg("fcntl()");

					wrapEvSet(_event_list, new_client, EVFILT_READ, EV_ADD | EV_ENABLE);
					wrapEvSet(_event_list, new_client, EVFILT_WRITE, EV_ADD | EV_ENABLE);
					this->getServer().setFdFlags(new_client);
				}
				else if (servReceive(_monitor[i].ident)) {
					int idx;
					while ((idx = findCrln(_msg_map[_monitor[i].ident].first)) != -1) {
						std::string test =_msg_map[_monitor[i].ident].first.substr(0, idx);
						std::cout << "recv data : " <<  test << "\n";
						std::pair<int, std::vector<std::string> > parsed_data = parseData(test);
						_msg_map[_monitor[i].ident].first.erase(0, idx + 2 - (_msg_map[_monitor[i].ident].first[i] == '\n'));
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
				if (this->getServer().getFdFlagsStatus(_monitor[i].ident, 4) == true){
					this->getServer().removeFdFlags(_monitor[i].ident);
					close(_monitor[i].ident);
				}
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

	_msg_map[fd].first += std::string(buf);

	return (true);
}

void Handler::makeProtocol(int fd) {
	size_t delimiter;
	while ((delimiter = std::min(_msg_map[fd].first.find('\r'), _msg_map[fd].first.find('\n'))) != std::string::npos) {
		std::string data = _msg_map[fd].first.substr(0, delimiter);
		_msg_map[fd].first.erase(0, delimiter + 1);
		_msg_map[fd].second = data;
		if (data == "")
			break ;
		data.clear();
	}
}

void Handler::figureCommand(int fd, std::pair<int, std::vector<std::string> > &data) {
	Command *cmd = NULL;
	int ctype = data.first;
	std::string buf("");
	std::string name = getServer().getUserName(fd);

	if(this->getServer().getFdFlagsInitStatus(fd)) {
		if (ctype == JOIN)
			cmd = new Join(*this);
		else if (ctype == NICK)
			cmd = new Nick(*this);
		else if (ctype == QUIT)
			cmd = new Quit(*this);
		else if (ctype == PRIVMSG)
			cmd = new Privmsg(*this);
		else if (ctype == KICK)
			cmd = new Kick(*this);
		else if (ctype == PART)
			cmd = new Part(*this);
		else if (ctype == NOTICE)
			cmd = new Notice(*this);
		else if (ctype == USER)
			cmd = new User(*this);
		else if (ctype == PASS)
			cmd = new Pass(*this);
		else if (ctype == PING)
			cmd = new Ping(*this);
		else if (ctype == BOT)
			cmd = new Bot(*this);
	}else {
		if (ctype == PASS) {
			cmd = new Pass(*this);
		} else if (ctype == NICK) {
			cmd = new Nick(*this);
		} else if (ctype == USER) {
			cmd = new User(*this);
		}
	}
	if (cmd != NULL){
		cmd->run(fd, data.second);
		delete cmd;
	}
	else{
		if (ctype == WRONGARG) {
//			:irc.local 461 jujeon :Not enough parameters
			buf = ":";
			buf += SERVNAME;
			buf += ERR461 + name + MSG461;
			this->getServer().setFdMessage(fd, buf);
		}
		else if (ctype == INVAILDCMD) {
			buf = ":";
			buf += SERVNAME;
			buf += ERR421 + name + " " + data.second[0] + MSG421;
			this->getServer().setFdMessage(fd, buf);
		}
		else if (ctype == MODE || ctype == WHOIS || ctype == CAP)
			return;
		else if (this->getServer().getFdFlagsInitStatus(fd)) {
			buf = ":";
			buf += SERVNAME;
			buf += ERR451 + name + MSG451;
			this->getServer().setFdMessage(fd, buf);
		}
	}
}

Server &Handler::getServer(void) { return (_server); }