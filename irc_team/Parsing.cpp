#include "Parsing.h"
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>


//__inline std::map<int, std::ve emptystring(void);
// 
// msgcase(buf);

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
	{	// deal with cmds
		new_string.str(buf);	
		std::getline(new_string, cmd, ' ');
		new_string.clear();

		cmd_type = checkCommand(cmd);
		std::cout << buf.size() << ' ' << cmd.size() << '\n';
		if (buf.size() == cmd.size())
			args = "";
		else
			args = buf.substr(cmd.size() + 1);
		new_string.str(args);

		if (cmd_type == EMPTY || cmd_type == WRONGARGU || cmd_type == INVAILDCMD)
			;
		else if (cmd_type == PRIVMSG)
			; /* PRIVMSG split */
		else
		{
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
		else if (cmd_type == KICK || cmd_type == NICK || cmd_type == JOIN)
		{	// KICK NICK JOIN
			if (vector_size != 1)
				cmd_type = -2;
		}
		//

		/*
			// JOIN #channelname,#channelname
			// PART #channelname,#channelname
			std::vector<std::string> ft_split(std::vector<std::stirng> args)
			if (args.size() == 0)
				return (args);
			else{
				
				std::string temp = args.front();
				args.clear();
				iss.str(temp);
				while (std::getline(iss, buf, ','))
					args.pushback(buf);
				return (args);	
			}
			
		*/

		/*
			// PRIVMSG <nickname> :message
			if ret_vetor[1][0] != ':'
				/privmsg seowo:asdf asdf adsf
			
			std::getline(' ')
			std::vector.first() . find() ':' error 
			std::vector.second()[0] != ':'  error
			std::vecotr.second().size() == 1
				empty -3
			: substr(:_pos + 1) 
		*/
		// if (cmd_type == MSG)
		// {
		// 	// msg 인자갯수가 맞는지
		// 	if (ret_vector.size() != 2)
		// 		cmd_type == -2;
		// 	// msg start with ':
			
		// }
		// ret_map[checkCommand(cmd)] = argument;
		// return (ret_map);
		/*
		 * cmd <- 커맨드가 담김
		 * 커맨드 종류 체크가 필요하다.
		 * checkCommand(cmd)
		 */
		// new_string.clear();
		// test = buf.substr(cmd.size() + 1);
		// new_string.str(test);
		// while (std::getline(new_string, temp, ' '))
		// {
			
		// }

		// std::cout << "TEST " << test << '\n';
		// new_string.clear();
		// std::cout << cmd << '\n';
		// std::cout << ret_vector.size() << '\n';
		// if (ret_vector.size() == 0)
		// {
			
		// }
		// // 명령어 파싱 -> invalid 경우에는 return 
	
		// // (1) / 뒤부터 화이트스페이스까지 끊어온다.
		// // (2) 그리고 1~11에 맞는게 있는지 체크
		// // (2-1) 맞는게 있다면 리턴
		// // (2-2) 맞는게 없다면 에러다 에러 리턴	
		// ret_vector.push_back("");
		// ret_map[-3] = ret_vector;
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
	if (cmd == "/msg")
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
	parsing("/quit ");
	return (0);
}