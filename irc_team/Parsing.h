#ifndef _PARSING_H_
# define _PARSING_H_

enum{
	EMPTY = -3,
	NEEDMOREARGU = -2,
	NOTCMD = -1,
	JOIN = 0,
	PASS,
	NICK,
	USER,
	PING,
	QUIT,
	PRIVMSG,
	KICK,
	HELP,
	PART
};

#endif /* _PARSING_H_ */