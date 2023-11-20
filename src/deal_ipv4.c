//
// Created by wangyufan on 2023-11-20.
//

#include "yftcp.h"

extern void hz_process_udp(struct rte_mbuf *udp_memory_buffer,struct rte_ipv4_hdr *ipv4_header);

extern void hz_process_tcp(struct rte_mbuf *tcp_memory_buffer,struct rte_ipv4_hdr *ipv4_header);

extern void hz_process_icmp(struct rte_mbuf *icmp_memory_buffer,struct rte_ipv4_hdr *ipv4_header);

void
hz_process_with_ipv4(struct rte_mbuf *memory_buffer, struct rte_ipv4_hdr *ipv4_header) {
    switch (ipv4_header->next_proto_id) {
        case IPPROTO_UDP: {
            hz_process_udp(memory_buffer,ipv4_header);
            break;
        }

        case IPPROTO_TCP: {
            hz_process_tcp(memory_buffer,ipv4_header);
            break;
        }

        case IPPROTO_ICMP: {
            hz_process_icmp(memory_buffer,ipv4_header);
            break;
        }
        default:
            break;

    }
}
