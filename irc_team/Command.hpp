#ifndef __COMMAND_HPP_
# define __COMMAND_HPP_

#include "Handler.hpp"
#include "Db.hpp"

Db _db;

class Command{
	public :
		Command(Handler&);
		virtual void run(int, int, std::vector<std::string>) = 0;
		// Handler& getHandler(void);

	protected :
		Handler& _handler;
};

class Message : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Notice : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Join  : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Nick : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Quit : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Privmsg : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Kick : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class Part : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

class List : public Command {
	public :
		void run(int, std::vector<std::string>);	
};

#endif /* __COMMAND_HPP_ */
