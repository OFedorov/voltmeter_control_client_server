#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <memory>
#include <netinet/in.h>

#include "device.h"

class Server{
public:
    Server(const uint16_t _port, int _socket_buffer_size = 4096);
    ~Server();

    int run();
    void setDevice(Device &_device);
    void stop();
    int getStatus();

    static const int not_created = -1;
    static const int created = 0;
    static const int up = 1;
    static const int stopped = 2;
    static const int socket_creat_error = 3;
    static const int socket_bind_error = 4;
    static const int socket_listen_error = 5;

private:
    class Client;
    uint16_t port;
    int server_socket;
    struct sockaddr_in server_adress;
    int status;
    size_t socket_buffer_size;
    std::vector<std::thread*> threads;
    std::vector<Client*> clients;

    Device *device;
    std::mutex m_device;

    void clientLoop(Client *client);

};

class Server::Client{
public:
    Client(int socket, size_t socket_buffer_size);
    ~Client();

    std::string receive_command_string();
    bool send_response(std::string response);
    void stop_receiving();
private:
    int socket;
    size_t socket_buffer_size;
    std::string request_buffer;

};

#endif //SEVER_H
