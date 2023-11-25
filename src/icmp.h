//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_ICMP_H
#define YFTCP_ICMP_H

#include "yftcp.h"

uint16_t hz_checksum(uint16_t *icmp_header_addr, int count);

void
hz_build_icmp_request(uint8_t *msg, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac, uint32_t sip,
                      uint32_t dip, uint16_t id,
                      uint16_t seq_num);

struct rte_mbuf *
hz_send_icmp(struct rte_mempool *memory_buffer_pool, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac,
             uint32_t sip,
             uint32_t dip, uint16_t id,
             uint16_t seq_num);

void hz_process_icmp(struct rte_mempool *memory_buffer_pool, struct rte_ether_hdr *ether_header,
                     struct rte_ipv4_hdr *ipv4_header);

#endif //YFTCP_ICMP_H
