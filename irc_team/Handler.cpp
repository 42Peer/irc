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

                    // create new user
                    struct s_user_info new_client_info;
                    std::vector<std::string> temp_ch;
                    new_client_info.fd = new_client;
                    new_client_info.pass = "";
                    new_client_info.name = "";
                    new_client_info.nick = "";
                    new_client_info.channel_list = temp_ch;

                    _fd_authorized[new_client] = false;
                    _before_auth[new_client] = new_client_info;
                }
                else
                {
                    if (servReceive(_monitor[i].ident))
                    {
                        std::cout << "RECV" << std::endl;
                        makeProtocol(_monitor[i].ident);
                    }
                }
            }
            else if (_monitor[i].filter == EVFILT_WRITE)
            {
                for (unsigned long j = 0; j < _msgMap.size(); ++j)
                {
                    // if (_fd_authorized[_monitor[j].ident] == true)
                    //     return;
                    if (_msgMap[_monitor[j].ident] == "")
                        continue;

                    for (std::map<int, std::string>::iterator it = _msgMap.begin(); it != _msgMap.end(); ++it){
                        send(it->first, _msgMap[_monitor[j].ident].c_str(), _msgMap[_monitor[j].ident].length() + 1, 1);
                        std::cout << _msgMap[_monitor[j].ident] << std::endl;
                    }

                    _msgMap[_monitor[j].ident] = "";
                }
            }
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

    buf[buf_len] = '\0';
 
    _msgMap[fd] += std::string(buf);

    return true;
}

void Handler::makeProtocol(int fd)
{

    size_t delimiter;
	while ((delimiter = std::min(_msgMap[fd].find('\r'), _msgMap[fd].find('\n'))) != std::string::npos)
    {
        std::string cmd = _msgMap[fd].substr(0, delimiter);
        _msgMap[fd].erase(0, delimiter + 1);
        if (cmd == "") continue;

        if (_fd_authorized[fd] == false)
        {
            auth(cmd, fd);
        }
        else
        {
            std::pair<int, std::vector<std::string> > testttttt = parseData(cmd);
            figureCommand(fd, testttttt);
        }
    }
    // runCmd() => 채널에 보낸건지 아닌지 ,특정 대상인지 체크해서
    // 해당 _msgMap[result] = _msgMap[_monitor[i[].ident].substr(0, _msgMap[_monitor[i].ident].find("\r\n"));
    // _msgMap[_monitor[i].ident] = _msgMap[_monitor[i].ident].substr(_msgMap[_monitor[i].ident].find("\r\n"));
}

void Handler::auth(std::string ret, int fd)
{
    if (ret == "")
        return;

    // temp parser
    std::string buff;
    std::vector<std::string> vec;
    std::istringstream ss(ret);

    while (std::getline(ss, buff, ' '))
        vec.push_back(buff);

    if (vec.size() < 2)
    {
        std::cout << "not enough args (>= 2)\n";
        return;
    }

    if (vec[0] == "PASS" && _before_auth[fd].pass == "")
    {
        if (_server.getServerPassword() == vec[1])
        {
            _before_auth[fd].pass = vec[1];
        }
    }

    if (vec[0] == "NICK" && _before_auth[fd].nick == "")
    {
        _before_auth[fd].nick = vec[1];
    }
    if (vec[0] == "USER" && _before_auth[fd].name == "")
    {
        if (vec.size() < 4)
        {
            std::cout << "not enough args (>= 4)\n";
            return;
        }
        // USER dohyun 0 * realdohyun
        _before_auth[fd].name = vec[1] + " " + vec[4];
        _fd_authorized[fd] = true;
        this->_server.g_db.addUser(_before_auth[fd]);
        this->_server.setMapData(fd, _before_auth[fd].nick);
        send(fd, "# Welcome!\n", 12, 0);
    }
}

Server &Handler::getServer(void)
{
    return (_server);
}

void Handler::figureCommand(int fd, std::pair<int, std::vector<std::string> > &data)
{
    if (data.first < 0)
        ;
    else
    {
        std::cout << "TEST\n";
        Command *cmd = NULL;
        switch (data.first)
        {
        case MESSAGE:
            cmd = new Message(*this);
            break;
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
        default:;
            // delete cmd; /* somthing wrong */
        }
        cmd->run(fd, data.second);
        delete cmd;
    }
}