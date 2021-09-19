#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <vector>

#include "channel.h"
#include "protocol.h"

std::string ok(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    return Protocol::OK + Protocol::ENDER;
}

std::string fail(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    return Protocol::FAIL + Protocol::ENDER;
}

std::string runtime_error(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    throw -1;
    return Protocol::FAIL + Protocol::ENDER;
}

std::string start_measure(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    if (params_list.size() != 0){
        return Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
    }
    channel->update();
    std::string result;
    if (channel->status == CHANNEL::idle_state){
        channel->status = CHANNEL::measure_state;
        result = Protocol::OK + Protocol::ENDER;
    } else {
        result = Protocol::FAIL + Protocol::ENDER;
    }
    return result;
}

std::string stop_measure(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    if (params_list.size() != 0){
        return Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
    }
    channel->update();
    std::string result;
    if (channel->status == CHANNEL::measure_state || channel->status == CHANNEL::error_state){
        channel->status = CHANNEL::idle_state;
        result = Protocol::OK + Protocol::ENDER;
    } else {
        result = Protocol::FAIL + Protocol::ENDER;
    }
    return result;
}

std::string get_status(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    if (params_list.size() != 0){
        return Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
    }
    std::string result;
    result = Protocol::OK + Protocol::COMMA;
    result += channel->status;
    result += Protocol::ENDER;
    return result;
}

std::string set_range(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    if (params_list.size() != 1){
        return Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
    }
    int range = Protocol::parse_range(params_list[0]);
    if (range == -1){
        return Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_PARAMETERS + Protocol::ENDER;
    }
    channel->update();
    std::string result;
    if (channel->status == CHANNEL::idle_state){
        channel->range = range;
        result = Protocol::OK + Protocol::COMMA + Protocol::RANGE;
        result += std::to_string(range);
        result += Protocol::ENDER;
    } else {
        result = Protocol::FAIL + Protocol::COMMA + Protocol::RANGE;
        result += std::to_string(range);
        result += Protocol::ENDER;
    }
    return result;
}

std::string get_result(CHANNEL::Channel *channel, std::vector<std::string> params_list){
    if (params_list.size() != 0){
        return Protocol::FAIL + Protocol::COMMA + Protocol::WRONG_NUMBER_OF_PARAMETERS + Protocol::ENDER;
    }
    channel->update();
    std::string result;
    if (channel->status == CHANNEL::measure_state){
        result = Protocol::OK + Protocol::COMMA + Protocol::RESULT;
        result += std::to_string(channel->value);
        result += Protocol::ENDER;
    } else {
        result = Protocol::FAIL + Protocol::ENDER;
    }
    return result;
}



#endif // COMMANDS_H
