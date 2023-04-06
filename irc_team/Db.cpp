#include "Db.hpp"

int main() {
    ChannelData channel;

    channel.setUser("hi",0, 0);
    cout << channel.getAllUser() << std::endl;
}
