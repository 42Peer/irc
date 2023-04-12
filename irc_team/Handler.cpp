#include "Handler.hpp"
void printErrorMsg(const char *msg) {
	std::cerr << "Error : " << msg << '\n';
	exit(1);
}
void wrapEvSet(std::vector<struct kevent>& list, int ident, int filter, int flag) {
    // We don't know parameter udata when we use
	struct kevent new_event;
	EV_SET(&new_event, ident, filter, flag, 0, 0, 0);
	list.push_back(new_event);
}

Handler::Handler(Server& server_): _server(server_) {
    _kq = kqueue();
    if (_kq == -1)
        printErrorMsg("kqueue()");
    wrapEvSet(_event_list, _server.getServerSocket(), EVFILT_READ, EV_ADD | EV_ENABLE);
}

Handler::~Handler() {}


void Handler::run(void) {
    std::cout << "socket server running...\n";

    int evt;
    while (true) {
        sleep(1);
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
            // readFilter가 있는 애들 ( 서버, 클라이언트 )
            else if (_monitor[i].filter == EVFILT_READ)
            {
                // 서버면,
                if (_monitor[i].ident == _server.getServerSocket())
                {
                    // 새 소켓 만들고
                    socklen_t sock_len = sizeof(sockaddr_in);
                    int new_client = accept(_server.getServerSocket(), (struct sockaddr*)(&_server.getServerAddr()), &sock_len);
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

                    _msgMap[new_client] = "PASSWORD : ";
                    // this->_server.setUserInfo(new_client, "TESTNAME", "TESTNAME");
                    std::cout << "# new client! : " << new_client << '\n';
                }
                // 클라이언트면,
                else
                {

                    std::string ret;
                    char buf[1024];
                    int r;

                    memset(buf, 0, sizeof(buf));
                    
                    while ((r = read(_monitor[i].ident, buf, 1024)) > 0)
                    {
                        buf[r] = 0;
                        ret += buf;
                    }

                    // // pass가 아니면,
                    // _msgMap[_monitor[i].ident] = ret;


                    ret = ret.substr(0, ret.size() - 1);
                    std::cout << "# pass : _" << ret << "_\n";

                    if(ret == "1234")
                        std::cout << "# correct : _" << ret << "_\n";


                    _msgMap[_monitor[i].ident] = "";



                    // memset(buf, 0, sizeof(buf));
                    // // write(new_client, "PASSWORD: ", 11);
                    // read(new_client, buf, sizeof(buf));
                    // std::string pass(buf);
                    // pass = pass.substr(0, pass.size() - 1);
                    // std::cout << "PASS: " << pass << std::endl;
                    // if (pass == "") {
                    //     // write(new_client, "USER NAME: ", 12);
                    //     read(new_client, buf, sizeof(buf));
                    //     std::string tmp(buf);
                    //     tmp = pass.substr(0, tmp.size() - 1);
                    //     _msgMap[new_client] = tmp;
                    // }
                    // else {
                    //     std::cout << "[Error] Error!!!" << std::endl;
                    // }
                    // std::cout << "Success: " << new_client << " : " << _msgMap[new_client] << std::endl;


                    // map<int,vector<string>> = parsing(buf);
                    // int cmd_type = map.first;
                    // std::vector<std::string>> arguments = map.second;
                    // cmdExecute(cmd_type, arguments);
                    // func name == write ,switchCmd(); <- user , channel,
                    // run_cmd();
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

Server& Handler::getServer(void)
{
	return (_server);
}

// void figureCommand(std::pair<int, std::vector<std::string> > data)
// {
// 	if (data.first < 0)
// 		;
// 	else
// 	{
// 		Command* cmd;
// 		switch (data.first)
// 		{
// 			case MESSAGE :
// 				cmd = new Message();
// 				msg.run(data.second);
// 				break;
// 			case JOIN :

// 				break;
// 			case NICK :

// 				break;
// 			case QUIT :

// 				break;
// 			case PRIVMSG :

// 				break;
// 			case KICK :

// 				break;
// 			case PART :

// 				break;
// 			default :
// 				; /* somthing wrong */
// 		}
// 	}
// }