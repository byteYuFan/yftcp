//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_ARP_H
#define YFTCP_ARP_H

#include "yftcp.h"



void hz_process_with_arp(struct rte_mempool *memory_buffer_pool, struct rte_arp_hdr *arp_header);

void
hz_build_arp_request(uint8_t *msg, uint16_t option_code, struct rte_ether_addr *dst_mac, uint32_t sip, uint32_t dip);

struct rte_mbuf *
hz_send_arp(struct rte_mempool *memory_buffer_pool, uint16_t opcode, struct rte_ether_addr *dst_mac, uint32_t sip,
            uint32_t dip);


#endif //YFTCP_ARP_H
