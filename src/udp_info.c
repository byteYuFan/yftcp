//
// Created by wangyufan on 2023-11-21.
//

#include "udp_info.h"
#include "function.h"

struct hz_udp_payload *hz_create_udp_payload(uint32_t src_ip, uint32_t dst_ip, uint16_t src_port,
                                             uint16_t dst_port, int protocol, unsigned char *data,
                                             uint16_t data_length) {
    struct hz_udp_payload *new_udp_payload = rte_malloc(" struct udp_payload*", sizeof(struct hz_udp_payload), 0);
    if (!new_udp_payload)return NULL;
    new_udp_payload->src_ip = src_ip;
    new_udp_payload->dst_ip = dst_ip;
    new_udp_payload->src_port = src_port;
    new_udp_payload->dst_port = dst_port;
    new_udp_payload->protocol = protocol;
    new_udp_payload->data_length = data_length;
    // todo free
    new_udp_payload->data = (unsigned char *) malloc(sizeof(char) * data_length);
    if (!new_udp_payload->data)return NULL;
    memset(new_udp_payload->data, 0, data_length);
    if (data) rte_memcpy(new_udp_payload->data, data, data_length);
    return new_udp_payload;
}

void hz_print_udp_payload(struct hz_udp_payload *payload) {
    printf("Source IP: %s\n", hz_print_ipv4_address(payload->src_ip));
    printf("Destination IP: %s\n", hz_print_ipv4_address(payload->dst_ip));
    printf("Source Port: %u\n", ntohs(payload->src_port));
    printf("Destination Port: %u\n", ntohs(payload->dst_port));
    printf("Protocol: %d\n", payload->protocol);

    printf("Data: %s", payload->data);
    printf("\n");

    printf("Data Length: %u\n", payload->data_length);
}