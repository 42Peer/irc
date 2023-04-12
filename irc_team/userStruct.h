#ifndef USERSTRUCT_H
# define USERSTRUCT_H

#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

struct s_user_info {
  std::string nick;
  std::string name;
  int fd;
  std::vector<std::string> channel_list;
};

enum{
	FORMATERR = -4, /* wrong msg format or wrong channel format */
	WRONGARG = -3, /* wrong argument number */
	INVAILDCMD = -2,
	EMPTY = -1, /* no cmd or no argu */
	MESSAGE = 0,
	JOIN, /* arg : 1      delimeter : ',' */
	NICK, /* arg : 1 */
	QUIT, /* arg : 0 */
	PRIVMSG, /* arg : 2 */
	KICK, /* arg : 1 */
	PART, /* arg : 0,1    delimeter : ','  */
};

#endif /* USERSTRUCT_H */