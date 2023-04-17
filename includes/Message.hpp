#ifndef __MESSAGE_HPP_
# define __MESSAGE_HPP_

#define ERR401 "401 :No such nick or channel:\r\n"
#define ERR403 "403 :No such channel:\r\n"
#define ERR404 "404 :Cannot send to channel:\r\n"
#define ERR407 "407 :Duplicate recipients. No message delivered:\r\n"
#define ERR412 "412 :No text to send:\r\n"
#define ERR421 "421 :Unknown Command: "
#define ERR432 "432 :Erroneus nickname\r\n"
#define ERR433 "433 :Nickname is already in use:\r\n"
#define ERR441 "441 :They aren't on that channel\r\n"
#define ERR442 "442 :You're not on that channel:\r\n"
#define ERR451 "451 :You have not registered:\r\n"
#define ERR461 "461 :Not enough parameters:\r\n"
#define ERR462 "462 :You may not register:\r\n"
#define ERR476 "476 :Invalid channel name:\r\n"
#define ERR482 "482 :You're not channel operator\r\n"

#define MSGNOTICE " NOTICE :"
#define MSGJOIN " JOIN :"
#define MSGNICK " NICK :"
#define MSGQUIT " QUIT :"
#define MSGPRIVMSG " PRIVMSG :"
#define MSGKICK " NICK :"
#define MSGPART " PART :"
#define MSGGREETING ":Welcome to IRC server\r\n"
#define CODEGREET "001 "

#endif /* __MESSAGE_HPP_ */
