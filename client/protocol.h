#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <string>
#include <vector>

//ok fail channel range result

namespace Protocol {
    //keywords
    const std::string OK = "ok";
    const std::string FAIL = "fail";
    const std::string GAP = " ";
    const std::string COMMA = ", ";
    const std::string ENDER = "\n";
    const std::string CHANNEL = "channel";
    const std::string RANGE = "range";
    const std::string RESULT = "result";

    //errors
    const std::string UNKNOWN_COMMAND = "unknown_command";
    const std::string WRONG_CHANNEL = "wrong_channel";
    const std::string WRONG_PARAMETERS = "wrong_parameters";
    const std::string WRONG_NUMBER_OF_PARAMETERS = "wrong_number_of_parameters";
    const std::string RUNTIME_ERROR = "runtime_error";

    //device commands
    const std::string GET_COMMAND_LIST = "get_command_list";
    const std::string GET_CHANNEL_NUMBER = "get_channel_number";

    struct Command{
        Command();
        std::string command_name;
        int channel_number;
        std::vector<std::string> params;
    };

    struct Response{
        Response();
        std::string status;
        std::vector<std::string> results;
    };

    Command parse_command(std::string command_str);
    Response parse_response(std::string response_str);
    std::vector<std::string> split_string(std::string str);
    int parse_range(std::string range_str);
    int parse_channel(std::string channel_str);
    float parse_result(std::string result_str);
    bool is_result(std::string result_str);
    bool is_float(std::string float_str);
    bool is_digit(std::string digit_str);


}



#endif // PROTOCOL_H
