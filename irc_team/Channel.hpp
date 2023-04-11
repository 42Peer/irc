#ifndef __CHANNEL_HPP_
# define __CHANNEL_HPP_

#include "Server.hpp"

class Channel{
	public :
		// Channel();
		// ~Channel();
		void	setList(std::string, std::string);
		std::vector<std::string> getUserList(std::string);
		void	changeNameFromChannelList(std::string, std::string, std::string);
		void	removeUserFromChannel(std::string, std::string);
		bool	isExistInChannel(std::string, std::string);

	private :
		std::map<std::string, std::vector<std::string> > _channel_nick_list;
};


#endif /* __CHANNEL_HPP_ */