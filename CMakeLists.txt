cmake_minimum_required(VERSION 3.30)
project(TP10_Sockets_IPV6 C)

set(CMAKE_C_STANDARD 11)

add_executable(TP10_Sockets_IPV6
        src/client/client.c
        src/server/server.c)