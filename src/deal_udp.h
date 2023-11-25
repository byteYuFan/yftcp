//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_DEAL_UDP_H
#define YFTCP_DEAL_UDP_H

#include "yftcp.h"

void
hz_build_udp_request(uint8_t *msg, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac, uint32_t sip,
                     uint32_t dip, uint16_t src_port, uint16_t dst_port, unsigned char *data, uint16_t total_length);
struct rte_mbuf *hz_send_udp(struct rte_mempool*memory_buffer_pool, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac, uint32_t sip,
                             uint32_t dip, uint16_t src_port, uint16_t dst_port, unsigned char *data, uint16_t udp_data_length);

#endif //YFTCP_DEAL_UDP_H
