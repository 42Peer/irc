#include "Db.hpp"

//bool ChannelData::findUser(const std::string& key) {
//        std::map<std::string, int>::iterator iter = _tables.begin();
//        for (; iter != _tables.end(); ++iter) {
//            if (iter->first == key) {
//               return (true);
//            }
//        }
//        return (false);
//    }
//
//void Db::channelClear(const std::string& key)
//{
//    std::map<std::string, ChannelData>::iterator channelIter = channel_tables.find(key);
//    if (channelIter != channel_tables.end() && channelIter->second.isEmpty())
//	    channel_tables.erase(channelIter);
//}