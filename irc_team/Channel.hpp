#ifndef __CHANNEL_HPP_
# define __CHANNEL_HPP_

#include "Server.hpp"

class Channel{
	public :
		Channel(std::string);
		~Channel();
		int addUser(struct s_user_info);
		int deleteUser(struct s_user_info);
	private :
		std::string					_channel_name;
		std::map<std::string, int>	_channel_user_list;
};

#endif /* __CHANNEL_HPP_ */