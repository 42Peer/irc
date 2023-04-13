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
    // We don't know parameter udata when we use
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
    std::cout << "socket server running...\n";
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
            // readFilter가 있는 애들 ( 서버,  클라이언트 )
            else if (_monitor[i].filter == EVFILT_READ)
            {
                // 서버면,
                if (_monitor[i].ident == _server.getServerSocket())
                {
                    // 새 소켓 만들고
                    socklen_t sock_len = sizeof(sockaddr_in);
                    int new_client = accept(_server.getServerSocket(), (struct sockaddr *)(&_server.getServerAddr()), &sock_len);
                    if (new_client == -1)
                        continue;
                    // 옵션 주고
                    setsockopt(new_client, SOL_SOCKET, SO_REUSEADDR, 0, 0);
                    // 논 블록 소켓으로 설정
                    if (fcntl(new_client, F_SETFL, O_NONBLOCK) == -1)
                        printErrorMsg("fcntl()");

                    // 새 클라이언트가 READ, WRITE 이벤트 가능하게 설정
                    wrapEvSet(_event_list, new_client, EVFILT_READ, EV_ADD | EV_ENABLE);
                    wrapEvSet(_event_list, new_client, EVFILT_WRITE, EV_ADD | EV_ENABLE);

                    // this->_server.setUserInfo(new_client, "TESTNAME", "TESTNAME");
                    std::cout << "# new client attempting... : " << new_client << '\n';

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

                    // this->_server.g_db.addUser(testsetst);
                    // this->_server.setMapData(new_client, temp_name);
                }
                else
                {
                    std::string ret;
                    char buf[1024];
                    int r;

                    memset(buf, 0, sizeof(buf));
                    // r = read(_monitor[i].ident, buf, 1024); 
                    // buf[r] = 0;
                    // _msgMap[_monitor[i].ident] += buf;
                    // _msgMap[_monitor[i].ident].find("\r\n") != std::string::npos ? runCmd();
                    // runCmd() => 채널에 보낸건지 아닌지 ,특정 대상인지 체크해서 
                    // 해당 _msgMap[result] = _msgMap[_monitor[i[].ident].substr(0, _msgMap[_monitor[i].ident].find("\r\n"));
                    // _msgMap[_monitor[i].ident] = _msgMap[_monitor[i].ident].substr(_msgMap[_monitor[i].ident].find("\r\n"));
                    while ((r = read(_monitor[i].ident, buf, 1024)) > 0)
                    {
                        buf[r] = 0;
                        ret += buf;
                    }
                    ret = ret.substr(0, ret.size() - 1);

                    if (_fd_authorized[_monitor[i].ident] == false)
                        auth(ret, i);

                    std::pair<int, std::vector<std::string> > testttttt = parseData(ret);
                    figureCommand(_monitor[i].ident, testttttt);
                }
            }
            // writeFilter가 있는 애들 ( 클라이언트 )
            else if (_monitor[i].filter == EVFILT_WRITE)
            {
                for (unsigned long j = 0; j < _msgMap.size(); ++j)
                {
                    if (_msgMap[_monitor[j].ident] == "")
                        continue;

                    for (std::map<int, std::string>::iterator it = _msgMap.begin(); it != _msgMap.end(); ++it)
                        send(it->first, _msgMap[_monitor[j].ident].c_str(), _msgMap[_monitor[j].ident].length() + 1, 1);

                    _msgMap[_monitor[j].ident] = "";
                }
            }
        }
    }
}

void Handler::auth(std::string ret, int i)
{
    if (ret == "")
        return ;
    ////////////////////////////////////////////////////
    // temp parser
    std::string buff;
    std::vector<std::string> vec;
    std::istringstream ss(ret);

    while (std::getline(ss, buff, ' '))
        vec.push_back(buff);

    if (vec.size() < 2)
    {
        std::cout << "not enough args (>= 2)\n";
        return ;
    }

    /////////////////////////////////////////////////////

    if (vec[0] == "PASS" && _before_auth[_monitor[i].ident].pass == "")
    {
        if (_server.getServerPassword() == vec[1])
        {
            // send(_monitor[i].ident, "# pass passed", 13, 0);
            _before_auth[_monitor[i].ident].pass = vec[1];
        }
    }

    if (vec[0] == "NICK" && _before_auth[_monitor[i].ident].nick == "")
    {
        // NICK dohyulee
        // send(_monitor[i].ident, "# nick passed", 13, 0);
        _before_auth[_monitor[i].ident].nick = vec[1];
    }
    if (vec[0] == "USER" && _before_auth[_monitor[i].ident].name == "")
    {
        if (vec.size() < 4)
        {
            std::cout << "not enough args (>= 4)\n";
            return ;
        }
        // USER dohyun 0 * realdohyun
            _before_auth[_monitor[i].ident].name = vec[1] + " " + vec[4];
            _fd_authorized[_monitor[i].ident] = true;
            _server.g_db.addUser(_before_auth[_monitor[i].ident]);
            send(_monitor[i].ident, "# Welcome!\n", 13, 0);
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
    }
}