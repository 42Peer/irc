#include "Channel.hpp"

extern Db g_db;

bool Channel::setList(std::string channel_name, std::string nick_name)
{
	if (_channel_nick_list.find(channel_name) == _channel_nick_list.end())
	{
		std::vector<std::string> temp_vector;
		temp_vector.push_back(nick_name);
		_channel_nick_list[channel_name] = temp_vector;
		return (true);
	}
	else
	{
		//duplication check needed?
		if (std::find(_channel_nick_list[channel_name].begin(), _channel_nick_list[channel_name].end(), nick_name)
							!= _channel_nick_list[channel_name].end())
			_channel_nick_list[channel_name].push_back(nick_name);
		return (false);
	}
};

std::vector<std::string>& Channel::getUserList(std::string channel_name)
{
	// if (_channel_nick_list.find(channel_name) == _channel_nick_list.end())
		// return (NULL);
	return (_channel_nick_list[channel_name]);
};

void Channel::changeNameFromChannelList(std::string channel, std::string old_name, std::string new_name) 
{
	std::vector<std::string>::iterator it = _channel_nick_list[channel].begin();
	std::vector<std::string>::iterator eit = _channel_nick_list[channel].end();
	std::vector<std::string>::iterator pos = std::find(it, eit, old_name);
	if (pos != eit)
	{
		_channel_nick_list[channel].erase(pos);
		_channel_nick_list[channel].push_back(new_name);
	}
};

void	Channel::removeUserFromChannel(std::string channel_name, std::string user_name) {
	std::vector<std::string>::iterator it = _channel_nick_list[channel_name].begin();
	std::vector<std::string>::iterator eit = _channel_nick_list[channel_name].end();
	std::vector<std::string>::iterator pos = std::find(it, eit, user_name);
	if (pos != eit)
		_channel_nick_list[channel_name].erase(pos);
};

bool	Channel::isExistInChannel(std::string channel_name, std::string target_name)
{
	std::vector<std::string>::iterator it = _channel_nick_list[channel_name].begin();
	std::vector<std::string>::iterator eit = _channel_nick_list[channel_name].end();
	std::vector<std::string>::iterator pos = std::find(it, eit, target_name);

	if (pos != eit)
		return (true);
	else
		return (false);
}