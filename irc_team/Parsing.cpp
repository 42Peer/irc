#include "Parsing.h"

int checkCommand(std::string);
int splitByComma(int, std::vector<std::string>&);
int splitMessageCommand(int, std::string, std::vector<std::string>&);
void splitOtherCommand(std::string, std::vector<std::string>&);
void deleteExeededArgs(std::vector<std::string>& v, int size, int& ctype);

std::pair<int, std::vector<std::string > > parseData(std::string buf)
{
	std::vector<std::string> ret_vector;
	std::pair<int, std::vector<std::string> > ret;
	int cmd_type(0);

	if (buf == "" || buf[0] == ' ')
	{
		ret_vector.push_back("");
		ret = std::pair<int, std::vector<std::string> >(EMPTY, ret_vector);
		return (ret);
	}else
	{
		std::string cmd, data;
		std::istringstream stream;
		int i(0);

		stream.str(buf);
		std::getline(stream, cmd, ' ');
		cmd_type = checkCommand(cmd);
		if (cmd_type == INVAILDCMD || cmd_type == CAP){
			ret_vector.push_back(buf);
			ret = std::pair<int, std::vector<std::string> >(cmd_type, ret_vector);
			return (ret);
		}
		else if (buf.size() == cmd.size())
			data = "";
		else
		{
			i = cmd.size();
			while (buf[i] != '\0' && buf[i] == ' ')
				++i;
			data = buf.substr(i);
		}

		if (cmd_type == PRIVMSG || cmd_type == NOTICE || cmd_type == QUIT || cmd_type == KICK)
			cmd_type = splitMessageCommand(cmd_type, data, ret_vector);
		else
			splitOtherCommand(data, ret_vector);
 
		if (cmd_type == QUIT && (ret_vector.size() != 1 && ret_vector.size() != 0))
			deleteExeededArgs(ret_vector, 1, cmd_type);
		else if ((cmd_type == JOIN || cmd_type == NICK || cmd_type == PART || cmd_type == PASS) && ret_vector.size() != 1)
			deleteExeededArgs(ret_vector, 1, cmd_type);
		else if (cmd_type == KICK && (ret_vector.size() != 2 && ret_vector.size() != 3))
			deleteExeededArgs(ret_vector, 3, cmd_type);
		else if (cmd_type == USER && ret_vector.size() != 4)
			deleteExeededArgs(ret_vector, 4, cmd_type);
		else if ((cmd_type == PRIVMSG || cmd_type == NOTICE) && ret_vector.size() != 2)
			deleteExeededArgs(ret_vector, 2, cmd_type);

		if (cmd_type == JOIN || cmd_type == PART || cmd_type == PRIVMSG)
			cmd_type = splitByComma(cmd_type, ret_vector);

		ret = std::pair<int, std::vector<std::string> >(cmd_type, ret_vector);
		return (ret);
	}
}

void deleteExeededArgs(std::vector<std::string>& v, int size, int& ctype)
{
	if (v.size() > size) {
		while(v.size() != size)
			v.pop_back();
		return ;
	}
	ctype = WRONGARG;
}

void splitOtherCommand(std::string data, std::vector<std::string>& args) {
	std::istringstream stream;
	std::string buf;	

	stream.str(data);
	while(std::getline(stream, buf, ' '))
		args.push_back(buf);
}

/* join part privmsg*/
int splitByComma(int ctype, std::vector<std::string>& args){
	std::istringstream stream;
	std::string targets;
	std::string message;

	if (args.size() == 0 || args.front().find(',') == std::string::npos)
		return (ctype);
	else if (ctype == PRIVMSG)
		message = args.back();
	
	stream.str(args.front());
	args.clear();
	while (std::getline(stream, targets, ','))
		args.push_back(targets);
	if (ctype == PRIVMSG)
		args.push_back(message);
	return (ctype);
}

/* QUIT PRIVMSG KICK NOTICE*/
int splitMessageCommand(int ctype, std::string data, std::vector<std::string>& args) {
	std::string targets;
	std::string msg;
	std::istringstream stream;
	int i(0);
	if (ctype == QUIT)
	{
		if (data == "")
			return (ctype);
		while (data[i] != '\0' && data[i] == ' ')
			++i;
	}
	else if (ctype  == PRIVMSG || ctype == NOTICE)
	{
		stream.str(data);
		std::getline(stream, targets, ' ');
		args.push_back(targets);
		i = targets.size();
		while (data[i] != '\0' && data[i] == ' ')
			++i;
	}
	else if (ctype == KICK)
	{
		stream.str(data);
		for(int k = 0; k < 2; ++k)
		{
			std::getline(stream, targets, ' ');
			args.push_back(targets);
			i += targets.size();
			while (data[i] != '\0' && data[i] == ' ')
				++i;
			targets.clear();
		}
	}
	msg = data.substr(i);
	if (msg == "" || msg[0] != ':')
		return (FORMATERR);
	args.push_back(msg.substr(1));
	return (ctype);
}

int checkCommand(std::string cmd)
{
	// for (std::string::iterator it = cmd.begin(); it != cmd.end(); ++it)
	// {
	// 		if (*it >= 65 && *it <= 90)
	// 			*it += 32;
	// }
	if (cmd == "NOTICE")
		return (NOTICE);
	else if (cmd == "JOIN")
		return (JOIN);
	else if (cmd == "NICK")
		return (NICK);
	else if (cmd == "PART")
		return (PART);
	else if (cmd == "QUIT")
		return (QUIT);
	else if (cmd == "PRIVMSG")
		return (PRIVMSG);
	else if (cmd == "KICK")
		return (KICK);
	else if (cmd == "PART")
		return (PART);
	else if (cmd == "PASS")
		return (PASS);
	else if (cmd == "USER")
		return (USER);
	else if (cmd == "CAP")
		return (CAP);
	else
		return (INVAILDCMD);
}

int main(int ac, char *av[])
{
	if (ac != 2)
		return (1);
	std::pair<int, std::vector<std::string> > ret = parseData(av[1]);
	std::vector<std::string>::iterator it = ret.second.begin();
	std::cout << "cmd type : " <<  ret.first << '\n';
	std::cout << "v size : " << ret.second.size() << '\n';
	if (ret.second.size() != 0){
		while (it != ret.second.end())
		{
			std::cout << *it << '\n';
			++it;
		}
	}
	return (0);
}