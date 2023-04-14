#include "Db.hpp"

bool ChannelData::findUser(const std::string& key) {
        std::map<std::string, int>::iterator iter = _tables.begin();
        for (; iter != _tables.end(); ++iter) {
            if (iter->first == key) {
               return (true);
            }
        }
        return (false);
    }