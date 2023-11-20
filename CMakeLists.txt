cmake_minimum_required(VERSION 3.26)
project(yftcp)

set(CMAKE_CXX_STANDARD 17)

add_executable(yftcp

        src/arp.c
        src/main.c
        src/yftcp.h
        src/function.h
        src/function.c
        src/config.h
        src/ring.c
        src/ring.h
        src/deal_packet.c
        src/deal_packet.h
        src/arp_table.c
        src/arp_table.h
        src/arp_table.c
        src/list.h
        src/timer.h
        src/timer.c
        src/udp.c
        src/udp.h
        src/deal_ipv4.c
        src/deal_ipv4.c
        src/rarp.c
        src/rarp.c
        src/tcp.c
        src/tcp.c
        src/icmp.c
        src/icmp.c)