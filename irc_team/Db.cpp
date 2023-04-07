#include "Db.hpp"

int main() {
    ChannelData channel;

    channel.addData(0, 12, "hi", "bye");
    channel.addData(1, 12, "hi2", "bye");
    channel.addData(1, 13, "hi", "good");
    channel.printTables();
}
