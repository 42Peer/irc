#ifndef __ETC_HPP_
#define __ETC_HPP_

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <memory.h>
#include <signal.h>
#include <stdlib.h>
#include <string>
#include <sys/_types/_socklen_t.h>
#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

typedef struct s_user_data {
  //   int socket_id;
  std::string user_name;
  std::string nick_name;
  std::vector<std::string> channel_lists;
  bool validation;
} t_user_data;

void printErrorMsg(const char *msg) {
  std::cerr << "Error : " << msg << '\n';
  exit(1);
}
#endif /* __ETC_HPP_ */