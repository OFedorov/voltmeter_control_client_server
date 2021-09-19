# Управление вольтметром

## Cервер
### Сборка и запуск
    
    $ mkdir server_build
    $ cd server_build
    $ cmake path_to/server
    $ make
    $ ./server [port] [number_of_channels]

## Клиент
### Сборка и запуск
    $ mkdir client_build
    $ cd client_build
    $ qmake path_to/client/client.pro
    $ make
    $ ./client
