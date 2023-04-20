#ifndef __PARSING_HPP_
# define __PARSING_HPP_

#include <vector>
#include <iostream>
#include <string>
#include <sstream>

enum {
	WRONGARG = -2, /* wrong argument number */
	INVAILDCMD = -1,
	NOTICE, /* arg : 2 */
	JOIN, /* arg : 1    channels  delimeter : ',' */
	NICK, /* arg : 1    nickname */
	QUIT, /* arg : 0,1  [reason] */
	PRIVMSG, /* arg : 2   "ch or user" :msg delimeter : ',' */
	KICK, /* arg : 2,3  channelname[,<channelname>] username[,<username>] [reason] */
	PART, /* arg : 1      delimeter : ',' channel */
	PASS, /* arg : 1 password */
	USER, /* arg : 4 username hostname servername realname */
	CAP,
	MODE,
	WHOIS,
	PING,
	WHO,
	BOT,
};

std::pair<int, std::vector<std::string> > parseData(std::string buf);

#endif /* __PARSING_HPP_ */
