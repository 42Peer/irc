#ifndef __COMMAND_HPP_
# define __COMMAND_HPP_

#include "Handler.hpp"


class Command{
	public :
		Command(Handler&);
		virtual void run(int, std::vector<std::string>) = 0;
		// Handler& getHandler(void);

	protected :
		Handler& _handler;
};

class Message : public Command {
	public :
		Message(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Notice : public Command {
	public :
		Notice(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Join  : public Command {
	public :
		Join(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Nick : public Command {
	public :
		Nick(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Quit : public Command {
	public :
		Quit(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Privmsg : public Command {
	public :
		Privmsg(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Kick : public Command {
	public :
		Kick(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

class Part : public Command {
	public :
		Part(Handler& h) : Command(h){};
		void run(int, std::vector<std::string>);	
};

#endif /* __COMMAND_HPP_ */
