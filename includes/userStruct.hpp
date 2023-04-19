#ifndef __USERSTRUCT_HPP_
#define __USERSTRUCT_HPP_

#include <string>
#include <vector>

struct s_user_info {
  int fd;
  std::string nick;
  std::string name;
  std::string real;
  std::vector<std::string> channel_list;
};

#endif /* __USERSTRUCT_HPP_ */