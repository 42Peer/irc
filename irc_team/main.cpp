#include "Server.hpp"
#include <cstdlib>
#include <string>

bool isArgv(const char *argv) {
  for (int i = 0; argv[i]; ++i) {
    if (!(argv[i] >= '0' && argv[i] <= '9'))
      return (false);
  }
  int num;
  if (num = atoi(argv)) {
    if (!(num >= 0 && num <= 65535)) {
      return (false);
    }
  }
  return (true);
}

int main(int argc, char **argv) {
  if (argc != 3) {
    return (1);
  }
  if (!isArgv(argv[1]))
    return (1);
  int port = atoi(argv[1]);
  std::string pass = argv[2];
  Server serv(atoi(argv[1]), argv[2]);
  return EXIT_SUCCESS;
}