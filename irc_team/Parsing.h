#ifndef _PARSING_H_
# define _PARSING_H_

enum{
	EMPTY = -3,
	WRONGARGU = -2,
	INVAILDCMD = -1,
	MESSAGE = 0,
	JOIN, /* arg : 1      delimeter : ',' */
	NICK, /* arg : 1 */
	QUIT, /* arg : 0 */
	PRIVMSG, /* arg : 2 */
	KICK, /* arg : 1 */
	PART, /* arg : 0,1    delimeter : ','  */
	LIST, /* arg : 0 */
};

#endif /* _PARSING_H_ */


/*
	인자 갯수를 먼저 체크한다?
	그리고 나서 커맨드 타입 구분한다.
	

*/