#include "Parsing.hpp"

void deleteExceededArgs(std::vector<std::string> &v, size_t size, int &ctype) {
  if (v.size() > size) {
	while (v.size() != size)
		v.pop_back();
	return;
  }
  ctype = WRONGARG;
}

void splitOtherCommand(std::string data, std::vector<std::string> &args) {
	std::istringstream stream;
	std::string buf("");
	std::string temp("");
	int flag(0);

	stream.str(data);
	while (std::getline(stream, buf, ' ')) {
	if (buf[0] == ':')
		flag = 1;
	if (!flag)
		args.push_back(buf);
	else {
		if (temp != "")
			temp.append(" ");
		temp.append(buf);
	}
	buf.clear();
  }
	if (flag){
	temp.erase(0, 1);
	args.push_back(temp);
  }
}

/* join part privmsg*/
int splitByComma(int ctype, std::vector<std::string> &args) {
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

int checkCommand(std::string cmd) {
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
	else if (cmd == "MODE")
		return (MODE);
	else if (cmd == "WHOIS")
		return (WHOIS);
	else if (cmd == "PING")
		return (PING);
	else if (cmd == "@BOT")
		return (BOT);
	else
		return (INVAILDCMD);
}

std::pair<int, std::vector<std::string> > parseData(std::string buf) {
	std::vector<std::string> ret_vector;
	std::pair<int, std::vector<std::string> > ret;
	int cmd_type(0);

	std::string cmd, data;
	std::istringstream stream;
	int i(0);

	stream.str(buf);
	std::getline(stream, cmd, ' ');
	cmd_type = checkCommand(cmd);
	if (cmd_type == INVAILDCMD || cmd_type == CAP || cmd_type == WHOIS || cmd_type == MODE) {
		ret_vector.push_back(cmd);
		ret = std::pair<int, std::vector<std::string> >(cmd_type, ret_vector);
		return (ret);
	} else if (buf.size() == cmd.size())
		data = "";
	else {
		i = cmd.size();
		while (buf[i] != '\0' && buf[i] == ' ')
		++i;
		data = buf.substr(i);
	}

	splitOtherCommand(data, ret_vector);

	if (cmd_type == QUIT && (ret_vector.size() != 1 && ret_vector.size() != 0))
		deleteExceededArgs(ret_vector, 1, cmd_type);
	else if ((cmd_type == JOIN || cmd_type == NICK || cmd_type == PART || cmd_type == PASS || cmd_type == PING) && ret_vector.size() != 1)
		deleteExceededArgs(ret_vector, 1, cmd_type);
	else if (cmd_type == KICK && (ret_vector.size() != 2 && ret_vector.size() != 3))
		deleteExceededArgs(ret_vector, 3, cmd_type);
	else if (cmd_type == USER && ret_vector.size() != 4)
		deleteExceededArgs(ret_vector, 4, cmd_type);
	else if ((cmd_type == PRIVMSG || cmd_type == NOTICE) &&	ret_vector.size() != 2)
		deleteExceededArgs(ret_vector, 2, cmd_type);

	if (cmd_type == JOIN || cmd_type == PART || cmd_type == PRIVMSG)
		cmd_type = splitByComma(cmd_type, ret_vector);

	ret = std::pair<int, std::vector<std::string> >(cmd_type, ret_vector);
	return (ret);
}