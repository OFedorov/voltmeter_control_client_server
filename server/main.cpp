#include <iostream>
#include <cstdint>
#include <csignal>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype>

#include "server.h"
#include "device.h"
//#include "channel.h"
#include "commands.h"
#include "logger.h"


static Server * signal_object;
extern "C" void signal_handler(int signum) {signal_object->stop();}

int main(int argc, char** argv){
    if (argc < 2){
        fprintf(stderr, "[MAIN] %s ERROR : %s\n", now_str().c_str(), "port number not specified");
        return -1;
    }
    uint16_t port;
    std::string port_str = argv[1];
    if (!std::all_of(port_str.begin(), port_str.end(), [](unsigned char c){ return std::isdigit(c); })){
        fprintf(stderr, "[MAIN] %s ERROR : %s%s\n", now_str().c_str(), "Invalid port number: ", argv[1]);
        return -1;
    }
    port = atoi(argv[1]);

    uint channel_number = 2;
    if (argc > 2){
        std::string channel_number_str = argv[2];
        if (!std::all_of(channel_number_str.begin(), channel_number_str.end(), [](unsigned char c){ return std::isdigit(c); })){
            fprintf(stderr, "[MAIN] %s ERROR : %s%s\n", now_str().c_str(), "Incorrect number of channels: ", argv[2]);
            return -1;
        }
        channel_number = atoi(argv[2]);
    }

    Server server(port);
    if (server.getStatus() != Server::created){
        fprintf(stderr, "[MAIN] %s ERROR : %s\n", now_str().c_str(), "failed to create server");
        return -1;
    }
    signal_object = &server;
    signal(SIGINT, signal_handler);

    Device device(channel_number);
    device.addCommand(std::string("get_status"), get_status);
    device.addCommand(std::string("start_measure"), start_measure);
    device.addCommand(std::string("stop_measure"), stop_measure);
    device.addCommand(std::string("get_result"), get_result);
    device.addCommand(std::string("set_range"), set_range);

    server.setDevice(device);

    if (server.run() < 0){
        fprintf(stderr, "[MAIN] %s ERROR : %s\n", now_str().c_str(), "failed to run server");
    }
    return 0;
}
