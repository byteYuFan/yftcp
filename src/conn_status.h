//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_CONN_STATUS_H
#define YFTCP_CONN_STATUS_H

#include "yftcp.h"
#include "config.h"
#include "ring.h"

typedef enum HZ_UDP_STATUS {
    HZ_UDP_CLOSED = 0,
    HZ_UDP_OPEN,
    HZ_UDP_CLOSE,
} HZ_UDP_STATUS;


typedef enum HZ_TCP_STATUS {
    HZ_TCP_STATUS_CLOSED = 0,
    HZ_TCP_STATUS_LISTEN,
    HZ_TCP_STATUS_SYN_REV,
    HZ_TCP_STATUS_SYN_SEND,
    HZ_TCP_STATUS_ESTABLISHED,

    HZ_TCP_STATUS_FIN_WAIT_1,
    HZ_TCP_STATUS_FIN_WAIT_2,
    HZ_TCP_STATUS_CLOSING,
    HZ_TCP_STATUS_TIME_WAIT,

    HZ_TCP_STATUS_CLOSE_WAIT,
    HZ_TCP_STATUS_LAST_ACK,

} HZ_TCP_STATUS;
struct hz_socket_info {
    uint32_t sip;
    uint32_t dip;
    uint16_t dst_port;
    uint16_t src_port;
    uint8_t protocol;
};

struct hz_connection_info {

    int fd;

    struct hz_socket_info *socket_info;
    struct rte_ether_addr src_mac;
    struct rte_ether_addr dst_mac;


    struct rte_ring *send_buffer;
    struct rte_ring *receive_buffer;

    union {
        // 添加其他TCP连接特有的字段
        struct {
            uint32_t send_nxt;
            uint32_t rcv_nxt;
            HZ_TCP_STATUS tcp_status;

        } tcp_info;

        struct {
            // 添加其他UDP连接特有的字段
            HZ_UDP_STATUS status;
        } udp_info;
    } special;

    // 其他 TCP 特有字段

    pthread_cond_t cond;
    pthread_mutex_t mutex;

    struct hz_connection_info *next;
    struct hz_connection_info *prev;
};


struct hz_connection_info *hz_init_connection_info(int fd, uint8_t protocol, uint32_t src_ip, uint32_t dst_ip,
                                                   uint16_t src_port, uint16_t dst_port,
                                                   struct rte_ether_addr src_mac, struct rte_ether_addr dst_mac);

void hz_destroy_connection_info(struct hz_connection_info *connection_info);

struct hz_socket_info *hz_create_socket_info(uint8_t protocol, uint32_t src_ip, uint32_t dst_ip,
                                             uint16_t src_port, uint16_t dst_port);

#endif //YFTCP_CONN_STATUS_H
