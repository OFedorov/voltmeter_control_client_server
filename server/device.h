#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <vector>
#include <map>
#include <functional>

#include "channel.h"

class Device{
public:
    Device(uint channel_number = 0);
    std::string execute(std::string command_str);
    void addCommand(std::string command_name, std::string (*command)(CHANNEL::Channel*, std::vector<std::string>));
    std::map<std::string, std::function<std::string(CHANNEL::Channel*, std::vector<std::string>)>> commands;
private:
    std::string getChannelsNumber();
    std::string getCommandList();
    std::vector<CHANNEL::Channel> channels;
};

#endif //DEVICE_H
