#include "Parsing.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

int checkCommand(std::string cmd);

std::map<int, std::vector<std::string > > parsing(std::string buf)
{
	std::vector<std::string> ret_vector;
	std::map<int, std::vector<std::string> > ret_map;
	std::string args, cmd, temp_arg;
	std::istringstream new_string;
	int cmd_type(0);

	if (buf == "" || buf[0] == ' ')
	{	// empty buf
		ret_vector.push_back("");
		ret_map[EMPTY] = ret_vector;
		return (ret_map);
	}
	else if (buf[0] != '/')
	{	// deal with message 
		ret_vector.push_back(buf);
		ret_map[MESSAGE] = ret_vector;
		return (ret_map);
	}
	// else if (buf[0] != '@')
	// {
		//bot
	// }
	else
	{	// deal with cmds, you will get cmd and args
		new_string.str(buf);	
		std::getline(new_string, cmd, ' ');
		cmd_type = checkCommand(cmd);
		std::cout << buf.size() << ' ' << cmd.size() << '\n';
		if (buf.size() == cmd.size()) // [/join] case
			args = "";				// prevent segfault
		else							// [/join ] case
			args = buf.substr(cmd.size() + 1);
		new_string.str(args);			// new_string <- grap sentences without cmd

		if (cmd_type == EMPTY || cmd_type == WRONGARG || cmd_type == INVAILDCMD)
			;
		else if (cmd_type == PRIVMSG)
		{	// process PRIVMSG here
			std::string nick_name;
			std::string msg;
			std::getline(new_string, nick_name, ' ');
			if (nick_name.find(':') != nick_name.npos)
				; // error
			else
			{
				ret_vector.push_back(nick_name);
                msg = args.substr(nick_name.size() + 1);
                size_t i(-1);
                while(msg[++i])
                {
                    if (msg[i] != ' ')
                        break ;
                }
                msg = msg.substr(i);
                if (msg == "" || (msg[0] == ':' && msg.size() == 1))
                    cmd_type = EMPTY;
                else if (msg[0] != ':')
                    cmd_type = WRONGARG;
                else {
                    msg = msg.substr(1);
                    ret_vector.push_back(msg);
                }
			}
		}
		else
		{	// process another cmds here
			std::cout << ret_vector.size() << " START getline\n";
			while (std::getline(new_string, temp_arg, ' '))
				ret_vector.push_back(temp_arg);
		}

		int vector_size = ret_vector.size();
		if (cmd_type == PART)
		{	// argu size 0, 1
			if (vector_size != 0 && vector_size != 1)
				cmd_type = -2;
		}
		else if (cmd_type == QUIT || cmd_type == LIST)
		{	// argu size 0
			if (vector_size != 0)
				cmd_type = -2;
		}
		else if (cmd_type == KICK || cmd_type == NICK || cmd_type == JOIN) {    // KICK NICK JOIN
            if (vector_size != 1)
                cmd_type = -2;
        }

        if (cmd_type == JOIN || cmd_type == PART) {
            if (ret_vector.size() == 0 )
                ;
            else
            {
                std::string temp = *(ret_vector.begin());
                if (temp.find(',') == std::string::npos)
                    ;
                else {
                    std::istringstream test_stream;
                    test_stream.str(temp);
                    ret_vector.clear();
                    std::string channel;
                    while (std::getline(test_stream, channel, ',')) {
                        std::cout << channel << '\n';
                        ret_vector.push_back(channel);
                    }
                    std::cout << "hi";
                }
            }
        }
		return (ret_map);
	}

}

int checkCommand(std::string cmd)
{
	// lower cmd
	for (std::string::iterator it = cmd.begin(); it != cmd.end(); ++it)
	{
		if (std::isalpha(*it))
			if (*it >= 65 && *it <= 90)
				*it += 32;
	}
	std::cout << "After convert : " << cmd << '\n';
	if (cmd == "/join")
		return (JOIN);
	if (cmd == "/nick")
		return (NICK);
	if (cmd == "/part")
		return (PART);
	if (cmd == "/quit")
		return (QUIT);
	if (cmd == "/privmsg")
		return (PRIVMSG);
	if (cmd == "/list")
		return (LIST);
	if (cmd == "/kick")
		return KICK;
	return (INVAILDCMD);
}

/* 
	핸들러에서 파싱함수에게 아래와같이 넘겨준다.
	string buf로 할당되어있고,
	read (socket, buf, ) 하게되고
	parsing ( buf ) 로 받게된다.
	
	여기서 
*/

int main(void)
{
  	parsing("/join #123,#1234,#4321");
	return (0);
}