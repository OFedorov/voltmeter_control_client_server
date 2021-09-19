#include "server.h"

#include <sys/socket.h>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <ctime>
#include <cstdio>

#include "logger.h"

Server::Server(const uint16_t _port, int _socket_buffer_size) : port(_port),
                                                                socket_buffer_size(_socket_buffer_size),
                                                                status(not_created),
                                                                device(nullptr){
    server_adress.sin_family = AF_INET;
    server_adress.sin_port = htons(port);
    server_adress.sin_addr.s_addr = htons(INADDR_ANY);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0){
        status = socket_creat_error;
        fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(), "can not create socket");
        return;
    }
//    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "socket created");

    if (bind(server_socket, reinterpret_cast<struct sockaddr*>(&server_adress), sizeof(server_adress)) == SO_ERROR){
        status = socket_bind_error;
        fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(),"can not bind socket");
        return;
    }
//    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "socket binded");

    if (listen(server_socket, 5) < 0){ // todo: set backlog
        status = socket_listen_error;
        fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(), "can not listen socket");
        return;
    }
//    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "socket listened");
    status = created;
    fprintf(stdout, "[Server] %s : server created no port: %i\n", now_str().c_str(), port);
}

Server::~Server()
{

}

int Server::run(){
    if (device == nullptr){
        fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(), "no device");
        return -1;
    }
    if (status != created){
        fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(), "Server not created");
        return -1;
    }
    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "START SERVER");
    status = up;
    socklen_t size = sizeof(server_adress);
    while (status == up){
        int client_socket = accept(server_socket, reinterpret_cast<struct sockaddr*>(&server_adress), &size);
        if (client_socket < 0){
            if (status != stopped){
                fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(), "can not accepting client");
            }
            continue;
        }
        Client *client = new Client(client_socket, socket_buffer_size);
        std::thread *thr = new std::thread(&Server::clientLoop, this , client);
        clients.push_back(client);
        threads.push_back(thr);
        fprintf(stdout, "[Server] %s : NEW CONNECTION Client ID %i\n", now_str().c_str(), client_socket);
    }
    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "STOP SERVER");
    for (Client *client: clients){
        client->stop_receiving();
    }
    for (std::thread *thr: threads){
        thr->join();
        delete thr;
    }
    for (Client *client: clients){
        delete client;
    }
    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "Bye");
    return 0;
}

void Server::setDevice(Device &_device)
{
    device = &_device;
    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "device added");
}

void Server::clientLoop(Client *client){
    while (status == up) {
        std::string command = client->receive_command_string();
        if (command.length() > 0){
            m_device.lock();
            std::string response = device->execute(command);
            m_device.unlock();
            if (!client->send_response(response)){
                break;
            }
        } else {
            break;
        }
    }
}

void Server::stop()
{
    fprintf(stdout, "[Server] %s : %s\n", now_str().c_str(), "close server socket");
    status = stopped;
    close(server_socket);
}

int Server::getStatus()
{
    return status;
}

Server::Client::Client(int socket, size_t socket_buffer_size): socket(socket), socket_buffer_size(socket_buffer_size)
{
    fprintf(stdout, "[Server][Client ID %i] %s : %s\n", socket, now_str().c_str(), "connected");
}

Server::Client::~Client()
{
    fprintf(stdout, "[Server][Client ID %i] %s : %s\n", socket, now_str().c_str(), "close socket");
    close(socket);
}

std::string Server::Client::receive_command_string()
{
    std::string command;
    while (true) {
        size_t command_end = request_buffer.find('\n');
        if (command_end != std::string::npos){
            command = request_buffer.substr(0, command_end + 1);
            request_buffer.erase(0, command_end + 1);
            fprintf(stdout, "[Server][Client ID %i] %s : receved from client \"%s\"\n", socket, now_str().c_str(), CRton(command).c_str());
            break;
        }
        char buffer[socket_buffer_size];
        std::fill_n(buffer, socket_buffer_size, 0);
        int data_length = recv(socket, buffer, socket_buffer_size, 0);
//        LOG("[Server][Client ID %i] %t : received %i bytes", socket,  data_length);
        if (data_length == 0){
            fprintf(stdout, "[Server][Client ID %i] %s : %s\n", socket, now_str().c_str(), "reception stopped");
            break;
        }
        if (data_length == -1){
            fprintf(stdout, "[Server][Client ID %i] %s : %s\n", socket, now_str().c_str(), "client closed socket");
            break;
        }
        request_buffer += std::string(buffer, data_length);
    }
    return command;
}

bool Server::Client::send_response(std::string response)
{
    fprintf(stdout, "[Server][Client ID %i] %s : send to client \"%s\"\n", socket, now_str().c_str(), CRton(response).c_str());
    int total = 0;
    int n;
    int len = response.length();
    char buf[socket_buffer_size];
    strcpy(buf, response.c_str());
    while(total < len)
    {
        n = send(socket, buf+total, len-total, 0);
        if(n == -1) { break; }
        total += n;
    }
    if (n == -1){
        fprintf(stderr, "[Server] %s ERROR : %s\n", now_str().c_str(), "can not send a message to the client");
    }
    return (n==-1 ? false : true);
}

void Server::Client::stop_receiving()
{
    fprintf(stdout, "[Server][Client ID %i] %s : %s\n", socket, now_str().c_str(), "stop receiving");
    shutdown(socket, SHUT_RD);
}
