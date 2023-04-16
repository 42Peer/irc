#ifndef __MESSAGE_HPP_
# define __MESSAGE_HPP_

#define ERR401 "401 :No such nick or channel:\n"
#define ERR403 "403 :No such channel:\n"
#define ERR404 "404 :Cannot send to channel:\n"
#define ERR407 "407 :Duplicate recipients. No message delivered:\n"
#define ERR412 "412 :No text to send:\n"
#define ERR421 "421 :Unknown Command: "
#define ERR432 "432 :Erroneus nickname\n"
#define ERR433 "433 :Nickname is already in use:\n"
#define ERR442 "442 :You're not on that channel:\n"
#define ERR451 "451 :You have not registered:\n"
#define ERR461 "461 :Not enough parameters:\n"
#define ERR462 "462 :You may not register:\n"
#define ERR476 "476 :Invalid channel name:\n"
#define ERR482 "482 :You're not channel operator\n"

#define MSGNOTICE " NOTICE :"
#define MSGJOIN " JOIN :"
#define MSGNICK " NICK :"
#define MSGQUIT " QUIT :"
#define MSGPRIVMSG " PRIVMSG :"
#define MSGKICK " NICK :"
#define MSGPART " PART :"
#define MSGGREETING "001 :Welcome to IRC server:\n"

#endif /* __MESSAGE_HPP_ */
