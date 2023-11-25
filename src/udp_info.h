//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_UDP_INFO_H
#define YFTCP_UDP_INFO_H

#include "yftcp.h"

struct hz_udp_payload {
    uint32_t src_ip;
    uint32_t dst_ip;

    uint16_t src_port;
    uint16_t dst_port;

    int protocol;
    unsigned char *data;

    uint16_t data_length;
};

struct hz_udp_payload *hz_create_udp_payload(uint32_t src_ip, uint32_t dst_ip, uint16_t src_port,
                                             uint16_t dst_port, int protocol, unsigned char *data, uint16_t data_length);
void hz_print_udp_payload(struct hz_udp_payload *payload);
#endif //YFTCP_UDP_INFO_H
