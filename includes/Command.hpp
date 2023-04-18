#ifndef __COMMAND_HPP_
#define __COMMAND_HPP_

#include "Handler.hpp"
#include <sstream>

class Command {
public:
	Command(Handler & h) : _handler(h){};
	virtual ~Command() {};
	virtual void run(int, std::vector<std::string>) = 0;
	// Handler& getHandler(void);

protected:
	Handler &_handler;
};

class Notice : public Command {
public:
	Notice(Handler &h) : Command(h){};
	~Notice() {};
	void run(int, std::vector<std::string>);
};

class Join : public Command {
public:
	Join(Handler &h) : Command(h){};
	~Join() {};
	void run(int, std::vector<std::string>);
};

class Nick : public Command {
public:
	Nick(Handler &h) : Command(h){};
	~Nick() {};
	void run(int, std::vector<std::string>);
	bool isValidName(const std::string&);
};

class Quit : public Command {
public:
	Quit(Handler &h) : Command(h){};
	~Quit() {};
	void run(int, std::vector<std::string>);
};

class Privmsg : public Command {
public:
	Privmsg(Handler &h) : Command(h){};
	~Privmsg() {};
	void run(int, std::vector<std::string>);
};

class Kick : public Command {
public:
	Kick(Handler &h) : Command(h){};
	~Kick() {};
	void run(int, std::vector<std::string>);
private :
	std::vector<std::string> splitByComma(std::string);
};

class Part : public Command {
public:
	Part(Handler &h) : Command(h){};
	~Part() {};
	void run(int, std::vector<std::string>);
};

class User : public Command {
public:
	User(Handler &h) : Command(h){};
	~User(){};
	void run(int, std::vector<std::string>);
};

class Pass : public Command {
public:
	Pass(Handler &h) : Command(h){};
	~Pass(){};
	void run(int, std::vector<std::string>);
};

class Ping : public Command {
public:
	Ping(Handler &h) : Command(h){};
	~Ping(){};
	void run(int, std::vector<std::string>);
};

#endif /* __COMMAND_HPP_ */
