#ifndef _PARSING_H_
# define _PARSING_H_

#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>

enum{
	FORMATERR = -4, /* wrong msg format or wrong channel format */
	WRONGARG = -3, /* wrong argument number */
	INVAILDCMD = -2,
	EMPTY = -1, /* no cmd or no argu */
	MESSAGE = 0,
	NOTICE,
	JOIN, /* arg : 1      delimeter : ',' */
	NICK, /* arg : 1 */
	QUIT, /* arg : 0 */
	PRIVMSG, /* arg : 2 */
	KICK, /* arg : 1 */
	PART, /* arg : 0,1    delimeter : ','  */
	LIST, /* arg : 0 */
};

std::map<int, std::vector<std::string> > parseData(std::string);

#endif /* _PARSING_H_ */