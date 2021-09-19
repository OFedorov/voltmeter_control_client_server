#include "device.h"
#include "protocol.h"
#include "logger.h"
#include <cstdio>

Device::Device(uint channel_number)
{
    for (uint i = 0; i < channel_number; ++i) {
        channels.push_back(CHANNEL::Channel());
    }
    fprintf(stdout, "[Device] %s : device created whith %i channels\n", now_str().c_str(), channel_number);
}

std::string Device::execute(std::string command_str){
    Protocol::Command cmd = Protocol::parse_command(command_str);
    //DEVICE COMMANDS
    if (cmd.command_name == Protocol::GET_COMMAND_LIST){
        if (cmd.channel_number != -1 && cmd.params.size() != 0){
            return  Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
        }
        fprintf(stdout, "[Device] %s : execute \"%s\"\n", now_str().c_str(), Protocol::GET_COMMAND_LIST.c_str());
        return getCommandList();
    }
    if (cmd.command_name == Protocol::GET_CHANNEL_NUMBER){
        if (cmd.channel_number != -1 && cmd.params.size() != 0){
            return  Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
        }
        fprintf(stdout, "[Device] %s : execute \"%s\"\n", now_str().c_str(), Protocol::GET_CHANNEL_NUMBER.c_str());
        return getChannelsNumber();
    }
    //CHANNELS COMMANDS
    std::string result;
    if (commands.find(cmd.command_name) == commands.end()){
        result = Protocol::FAIL + Protocol::COMMA + Protocol::UNKNOWN_COMMAND + Protocol::ENDER;
    } else if (cmd.channel_number == -1 || cmd.channel_number > channels.size()-1) {
        result = Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_CHANNEL + Protocol::ENDER;
    } else {
        try {
            fprintf(stdout, "[Device] %s : execute command: \"%s\" for channel: %i\n", now_str().c_str(), cmd.command_name.c_str(), cmd.channel_number);
            result = commands[cmd.command_name](&channels[cmd.channel_number], cmd.params);
        } catch (...) {
            fprintf(stderr, "[Device] %s ERROR : runtime error of command \"%s\" for channel: %i\n", now_str().c_str(), cmd.command_name.c_str(), cmd.channel_number);
            result = Protocol::FAIL + Protocol::COMMA + Protocol::RUNTIME_ERROR + Protocol::ENDER;
        }
    }
    return result;
}

void Device::addCommand(std::string command_name, std::string (*command)(CHANNEL::Channel*, std::vector<std::string>) ){
    commands[command_name] = command;
    fprintf(stdout, "[Device] %s : added command \"%s\"\n", now_str().c_str(), command_name.c_str());
}

std::string Device::getChannelsNumber()
{
    std::string result = Protocol::OK;
    result += Protocol::COMMA;
    result += Protocol::CHANNEL;
    result += std::to_string(channels.size());
    result += Protocol::ENDER;
    return result;
}

std::string Device::getCommandList()
{    
    std::vector<std::string> keys;
    for(std::map<std::string, std::function<std::string(CHANNEL::Channel*, std::vector<std::string>)>>::iterator it = commands.begin();
        it != commands.end(); ++it) {
      keys.push_back(it->first);
    }
    std::string result;
    result = Protocol::OK;
    for (auto cmd : keys){
        result += Protocol::COMMA;
        result += cmd;
    }
    result += Protocol::ENDER;
    return result;
}


