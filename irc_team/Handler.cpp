#include "Handler.hpp"
#include "Command.hpp"
#include "Parsing.h"
#include "userStruct.h"

void printErrorMsg(const char *msg)
{
    std::cerr << "Error : " << msg << '\n';
    exit(1);
}
void wrapEvSet(std::vector<struct kevent> &list, int ident, int filter, int flag)
{
    struct kevent new_event;
    EV_SET(&new_event, ident, filter, flag, 0, 0, 0);
    list.push_back(new_event);
}

Handler::Handler(Server &server_) : _server(server_)
{
    _kq = kqueue();
    if (_kq == -1)
        printErrorMsg("kqueue()");
    wrapEvSet(_event_list, _server.getServerSocket(), EVFILT_READ, EV_ADD | EV_ENABLE);
}

Handler::~Handler() {}

void Handler::run(void)
{
    std::cout << "socket server running... fd : " << _server.getServerSocket() << "\n";
    std::map<int, std::string> tmp_data;

    int evt;
    while (true)
    {
        evt = kevent(_kq, &_event_list[0], _event_list.size(), _monitor, 8, NULL);
        if (evt == -1)
            printErrorMsg("evt()");
        _event_list.clear();

        for (int i = 0; i < evt; ++i)
        {
            if (_monitor[i].flags & EV_EOF)
            {
                std::cout << "Error: Client Disconnect\n";
                close(_monitor[i].ident);
            }
            else if (_monitor[i].flags & EV_ERROR)
            {
                std::cerr << "ENABLE\n";
                if (_monitor[i].ident == _server.getServerSocket())
                    printErrorMsg("Server error");
                else
                    std::cerr << "Client error\n";
            }
            else if (_monitor[i].filter == EVFILT_READ)
            {
                if (_monitor[i].ident == _server.getServerSocket())
                {
                    socklen_t sock_len = sizeof(sockaddr_in);
                    int new_client = accept(_server.getServerSocket(), (struct sockaddr *)(&_server.getServerAddr()), &sock_len);
                    if (new_client == -1)
                    {
                        std::cerr << "accept err\n";
                        continue;
                    }
                    std::cout << "# new client fd : " << new_client << '\n';

                    setsockopt(new_client, SOL_SOCKET, SO_REUSEADDR, 0, 0);
                    if (fcntl(new_client, F_SETFL, O_NONBLOCK) == -1)
                        printErrorMsg("fcntl()");

                    wrapEvSet(_event_list, new_client, EVFILT_READ, EV_ADD | EV_ENABLE);
                    wrapEvSet(_event_list, new_client, EVFILT_WRITE, EV_ADD | EV_ENABLE);

					_fd_flags[new_client] = 0;
                }
                else{
					servReceive(_monitor[i].ident);
                	makeProtocol(_monitor[i].ident);
				    std::pair<int, std::vector<std::string> > parsed_data = parseData(_msgMap[_monitor[i].ident].second);
				    figureCommand(_monitor[i].ident, parsed_data);
				}
            }
            else if (_monitor[i].filter == EVFILT_WRITE)
            {
                std::string fd_data = this->getServer().getFdMessage(_monitor[i].ident);
                if (fd_data != "")
                    send(_monitor[i].ident, fd_data.c_str(), fd_data.size(), 0); 
        }
    }
}

int Handler::servReceive(int fd)
{
    std::string ret;
    char buf[1024];
    int buf_len;

    memset(buf, 0, sizeof(buf));
    buf_len = recv(fd, (void *)buf, 1024, MSG_DONTWAIT);
    if (buf_len == 0)
        return false;
    // std::cout << buf << "\n";
    buf[buf_len] = '\0';

    _msgMap[fd].first += std::string(buf);

    return true;
}

void Handler::makeProtocol(int fd)
{
	size_t delimiter;
	while ((delimiter = std::min(_msgMap[fd].first.find('\r'), _msgMap[fd].first.find('\n'))) != std::string::npos)
	{
		std::string data = _msgMap[fd].first.substr(0, delimiter);
		_msgMap[fd].first.erase(0, delimiter + 1);
		_msgMap[fd].second = data;
		if (data == "")	continue;
		data.clear();
	}
}

Server &Handler::getServer(void)
{
    return (_server);
}

void Handler::figureCommand(int fd, std::pair<int, std::vector<std::string> > &data)
{
	if (_fd_flags.find(fd) != _fd_flags.end())
	{
		Command *cmd = NULL;
		if (data.first == PASS)
		{
			cmd = new Pass(*this);
			delete cmd;
		}
		else if (data.first == NICK && _fd_flags[fd] > 0)
		{
			cmd = new Nick(*this);
			delete cmd;
		}
		else if (data.first == USER && _fd_flags[fd] > 1)
		{
			cmd = new User(*this);
			delete cmd;
		}
		else
			;/* */
	}
    else
    {
        Command *cmd = NULL;
		switch (data.first){
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
			default:
				;
            // delete cmd; /* somthing wrong */
        	}
        cmd->run(fd, data.second);
        delete cmd;
    }
}

void    Handler::setFdFlags(int fd, int flags)
{
    if (this->_fd_flags.find(fd) != _fd_flags.end())
        _fd_flags[fd] = flags;
}