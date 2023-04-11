#ifndef USERSTRUCT_H
# define USERSTRUCT_H

#include <iostream>
#include <vector>

struct s_user_info {
  std::string nick;
  std::string name;
  std::vector<std::string> channel_list;
};

#endif /* USERSTRUCT_H */