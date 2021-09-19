#include "protocol.h"

#include <algorithm>
#include <cctype>
//#include <cstdio>

Protocol::Command::Command() : channel_number(-1)
{

}



Protocol::Command Protocol::parse_command(std::string command_str)
{
    if (command_str.find(ENDER) == std::string::npos){
        return Command();
    }
    std::vector<std::string> splited_command = split_string(command_str);
    if (splited_command.size() == 0){
        return Command();
    }
    Command cmd;
    cmd.command_name = splited_command[0];
    if (splited_command.size() == 1){
        return cmd;
    }
    cmd.channel_number = parse_channel(splited_command[1]);
    for (size_t i = 2; i < splited_command.size(); ++i){
        cmd.params.push_back(splited_command[i]);
    }
    return cmd;
}

Protocol::Response Protocol::parse_response(std::string response_str)
{
    if (response_str.find(ENDER) == std::string::npos){
        return Response();
    }
    std::vector<std::string> splited_response = split_string(response_str);
    if (splited_response.size() == 0){
        return Response();
    }
    std::string status = splited_response[0];
    if (status != Protocol::OK && status != Protocol::FAIL){
        return Response();
    }
    Response res;
    res.status = status;
    if (splited_response.size() > 1){
        for (size_t i = 1; i < splited_response.size(); ++i){
            res.results.push_back(splited_response[i]);
        }
    }
    return res;
}

std::vector<std::string> Protocol::split_string(std::string str)
{
    std::vector<std::string> splited_str;
    while (str.length() > 0) {
        size_t gap_pos = str.find(GAP);
        size_t comma_pos = str.find(COMMA);
        if (gap_pos != std::string::npos && gap_pos < comma_pos){
            std::string sub_cmd = str.substr(0, gap_pos);
            if (sub_cmd.length() > 0){
                splited_str.push_back(sub_cmd);
            }
            str.erase(0, gap_pos + 1);
            continue;
        }
        if (comma_pos != std::string::npos){
            std::string sub_cmd = str.substr(0, comma_pos);
            if (sub_cmd.length() > 0){
                splited_str.push_back(sub_cmd);
            }
            str.erase(0, comma_pos + 2);
            continue;
        }
        size_t ender_pos = str.find(ENDER);
        if (ender_pos != std::string::npos){
            std::string sub_cmd = str.substr(0, ender_pos);
            if (sub_cmd.length() > 0){
                splited_str.push_back(sub_cmd);
            }
            str.erase(0, ender_pos + 1);
            continue;
        }
    }
    return splited_str;
}

int Protocol::parse_channel(std::string channel_str)
{
    int channel_num = -1;
    size_t channel_str_pos = channel_str.find(CHANNEL);
    if (channel_str_pos != 0){
        return channel_num;
    }
    channel_str.erase(0, CHANNEL.length());
    if (is_digit(channel_str)){
        channel_num = stoi(channel_str);
    }
    return channel_num;
}

int Protocol::parse_range(std::string range_str)
{
    int range = -1;
    size_t channel_str_pos = range_str.find(RANGE);
    if (channel_str_pos != 0){
        return range;
    }
    range_str.erase(0, RANGE.length());
    if (is_digit(range_str)){
        range = stoi(range_str);
    }
    return range;
}


float Protocol::parse_result(std::string result_str)
{
    float result = 0.;
    size_t channel_str_pos = result_str.find(RESULT);
    if (channel_str_pos != 0){
        return result;
    }
    result_str.erase(0, RANGE.length());
    if (is_float(result_str)){
        result = stof(result_str);
    }
    return result;
}

Protocol::Response::Response()
{

}

bool Protocol::is_result(std::string result_str)
{
    size_t channel_str_pos = result_str.find(RESULT);
    if (channel_str_pos != 0){
        return false;
    }
    result_str.erase(0, RANGE.length());
    if (!is_float(result_str)){
        return false;
    }
    return true;
}

bool Protocol::is_float(std::string float_str)
{
    std::string::const_iterator it = float_str.begin();
    bool decimalPoint = false;
    int minSize = 0;
    if(float_str.size()>0 && (float_str[0] == '-' || float_str[0] == '+')){
      it++;
      minSize++;
    }
    while(it != float_str.end()){
      if(*it == '.'){
        if(!decimalPoint) decimalPoint = true;
        else break;
      }else if(!std::isdigit(*it) && ((*it!='f') || it+1 != float_str.end() || !decimalPoint)){
        break;
      }
      ++it;
    }
    return float_str.size()>minSize && it == float_str.end();
}

bool Protocol::is_digit(std::string digit_str)
{
    if (digit_str.empty()){
        return false;
    }
    return std::all_of(digit_str.begin(), digit_str.end(), [](unsigned char c){ return std::isdigit(c); });
}
