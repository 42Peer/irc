#include "Parsing.h"

int checkCommand(std::string);
int splitByComma(int, std::vector<std::string>&);
int splitPrivateNoticeCommand(int, std::string, std::vector<std::string>&);
void splitOtherCommand(std::string, std::vector<std::string>&);

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
	}
	else if (buf[0] != '/')
	{
		ret_vector.push_back(buf);
		ret = std::pair<int, std::vector<std::string> >(MESSAGE, ret_vector);
		return (ret);
	}
	else
	{
		std::string cmd, data;
		std::istringstream stream;

		/* 
			입력받은 buf 에서 Command 구분 후 타입체크 하여 cmd_type에 저장 후
			cmd에 커맨드 data에 남은 args를 저장
		*/
		stream.str(buf);
		std::getline(stream, cmd, ' ');
		cmd_type = checkCommand(cmd);
		if (buf.size() == cmd.size())
			data = "";
		else
			data = buf.substr(cmd.size() + 1);

		/* command 제외한 buf에서 argument 쪼개기 (':', ' ') */
		if (cmd_type == EMPTY || cmd_type == WRONGARG || cmd_type == INVAILDCMD)
			;
		else if (cmd_type == PRIVMSG || cmd_type == NOTICE)
			cmd_type = splitPrivateNoticeCommand(cmd_type, data, ret_vector);
		else
			splitOtherCommand(data, ret_vector);

		/* command type에 따른 argument 갯수 체크 후 에러 사항이 있을경우 cmd_type에 에러 값 저장 */
		if (cmd_type == PART)
		{
			if (ret_vector.size() != 0 && ret_vector.size() != 1)
				cmd_type = -2;
		}
		else if (cmd_type == QUIT)
		{
			if (ret_vector.size() != 0)
				cmd_type = -2;
		}
		else if (cmd_type == KICK || cmd_type == NICK || cmd_type == JOIN)
		{
			if (ret_vector.size() != 1)
				cmd_type = -2;
		}

		/* 콤마(',')를 기준으로 argument 쪼개기 */
		if (cmd_type == JOIN || cmd_type == PART)
			cmd_type = splitByComma(cmd_type, ret_vector);
		else if (cmd_type == PRIVMSG || cmd_type == NOTICE)
			cmd_type = splitByComma(cmd_type, ret_vector);
		/* 
			최종적으로 반환 할 map에 데이터 넣어서 execution 파트에서 cmd_type만 확인하여 
			에러일 경우는 vector부분 확인하지 않게끔
		*/
		ret = std::pair<int, std::vector<std::string> >(cmd_type, ret_vector);
		return (ret);
	}
}

void splitOtherCommand(std::string data, std::vector<std::string>& args) {
	std::istringstream stream;
	std::string buf;	

	stream.str(data);
	while(std::getline(stream, buf, ' '))
		args.push_back(buf);
}

int splitByComma(int ctype, std::vector<std::string>& args){
	std::istringstream stream;
	std::string channel;

	if (args.size() == 0)
		return (ctype);
	else if (ctype == JOIN || ctype == PRIVMSG)
	{
		std::string msg;
		
		if (args.front().find(',') == std::string::npos)
			return (ctype);
		else
		{
			stream.str(args.front());
			msg = args.back();
			args.clear();

			while (std::getline(stream, channel, ','))
			{
				if (channel[0] != '#')
					return (FORMATERR);
				args.push_back(channel);
			}
			args.push_back(msg);
			return (ctype);
		}
	}
	else 
	{
		if (args.front().find(',') == std::string::npos)
			return (ctype);
		else {
			stream.str(args.front());
			args.clear();

			while (std::getline(stream, channel, ','))
			{
				if (channel[0] != '#')
					return (FORMATERR);
				args.push_back(channel);
			}
			return (ctype);
		}
	}
}

int splitPrivateNoticeCommand(int ctype, std::string data, std::vector<std::string>& args) {
	std::string nick_name;
	std::string msg;
	std::istringstream stream;
	
	stream.str(data);
	std::getline(stream, nick_name, ' ');
	if (nick_name.find(':') != nick_name.npos)
		return (WRONGARG);
	else
	{
		args.push_back(nick_name);
		msg = data.substr(nick_name.size() + 1);
		size_t i(-1);
		while(msg[++i])
		{
			if (msg[i] != ' ')
				break ;
		}
		msg = msg.substr(i);
		if (msg == "" || (msg[0] == ':' && msg.size() == 1))
			return (EMPTY);
		else if (msg[0] != ':')
			return (FORMATERR);
		else {
			msg = msg.substr(1);
			args.push_back(msg);
			return (ctype);
		}
	}
}

int checkCommand(std::string cmd)
{
	for (std::string::iterator it = cmd.begin(); it != cmd.end(); ++it)
	{
			if (*it >= 65 && *it <= 90)
				*it += 32;
	}
	if (cmd == "/notice")
		return (NOTICE);
	if (cmd == "/join")
		return (JOIN);
	if (cmd == "/nick")
		return (NICK);
	if (cmd == "/part")
		return (PART);
	if (cmd == "/quit")
		return (QUIT);
	if (cmd == "/privmsg")
	{
		std::cout << "Ident\n";
		return (PRIVMSG);
	}
	if (cmd == "/kick")
		return (KICK);
	return (INVAILDCMD);
}

// int main(void)
// {
// 	std::cout << parseData("/join #123,#1234,#4321").first << '\n';
//     // parseData("/asdf");
//     // parseData("/pArt      ");
//     // parseData("/jOin");
//     // parseData("/privMsg jujeon,dllee : hi");
//     // parseData("/privMsg jujeon:hidasjifaji:sdjifsdfij");
//     // parseData("/privMsg jujeon:hidasjifaji :sdjifsdfij");
//     // parseData("/privMsg jujeon            :hi");
//     // parseData("/privMsg jujeon            :hi          ");
//     // parseData("/privMsg jujeon            :hi          ");
//     // parseData("/part");
//     // parseData("/part :jujeon");
//     // parseData("/part #123");
//     // parseData("/part #123,#2222");
//     // parseData("/part #123 , #2222");
//     return (0);
// }