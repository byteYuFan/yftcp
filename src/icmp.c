//
// Created by wangyufan on 2023-11-20.
//

#include "yftcp.h"

void hz_process_icmp(struct rte_mbuf *icmp_memory_buffer, struct rte_ipv4_hdr *ipv4_header) {
    struct rte_icmp_hdr *icmp_header = (struct rte_icmp_hdr *) (ipv4_header + 1);

}

void
hz_build_icmp_request(uint8_t *msg, struct rte_ether_addr *src_mac, struct rte_ether_addr *dst_mac, uint32_t sip,
                      uint32_t dip, uint16_t id,
                      uint16_t seq_num) {
    struct rte_ether_hdr *ether_header = (struct rte_ether_hdr *) msg;
    memcpy(ether_header->s_addr.addr_bytes, src_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    memcpy(ether_header->d_addr.addr_bytes, dst_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    ether_header->ether_type = htons(RTE_ETHER_TYPE_IPV4);

}