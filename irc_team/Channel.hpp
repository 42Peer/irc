#ifndef __CHANNEL_HPP_
# define __CHANNEL_HPP_

#include "etc.hpp"

class Channel{
	public :
		Channel(std::string);
		
		~Channel();
		int addUser(struct s_user_data);
		int deleteUser(struct s_user_data);
	private :
		std::string                 _channel_name;
		std::map<std::string, int>  _channel_user_list;
};
//유저가 quit으로 나간다 -> 채널에 유저가 없어진걸 서버가 확인 -> 0명일 경우 delete
/* 
_Server.deleteChannel(#123);
1 send 
quit -> _chan_u_l.empty(); return -1 -> figuerCMD -> run() -> Channel.delete(#123)

0
1
-1 
*/
#endif /* __CHANNEL_HPP_ */