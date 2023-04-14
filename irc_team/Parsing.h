#ifndef _PARSING_H_
# define _PARSING_H_

#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>

enum {
	FORMATERR = -4, /* wrong msg format format */
	WRONGARG = -3, /* wrong argument number */
	INVAILDCMD = -2,
	EMPTY = -1, /* no cmd or no argu */
	NOTICE, /* arg : 2 */
	JOIN, /* arg : 1    channels  delimeter : ',' */
	NICK, /* arg : 1    nickname */
	QUIT, /* arg : 0,1  [reason] */
	PRIVMSG, /* arg : 2   "ch or user" :msg delimeter : ',' */
	KICK, /* arg : 2,3  channelname username [reason] */
	PART, /* arg : 1      delimeter : ',' channel */
	PASS, /* arg : 1 password */
	USER, /* arg : 4 username hostname servername realname */
	CAP,
};

std::pair<int, std::vector<std::string> > parseData(std::string);

#endif /* _PARSING_H_ */