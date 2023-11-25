//
// Created by wangyufan on 2023-11-20.
//

#include "yftcp.h"
#include "ipv4.h"
#include "icmp.h"
#include "udp.h"
#include "tcp.h"

void
hz_process_with_ipv4(struct rte_mempool *memory_buffer_pool, struct rte_ether_hdr *ether_header,
                     struct rte_ipv4_hdr *ipv4_header) {
    switch (ipv4_header->next_proto_id) {
        case IPPROTO_UDP: {
            hz_process_udp(memory_buffer_pool, ether_header,ipv4_header);
            break;
        }

        case IPPROTO_TCP: {
            hz_process_tcp(memory_buffer_pool, ipv4_header);
            break;
        }

        case IPPROTO_ICMP: {
            hz_process_icmp(memory_buffer_pool, ether_header, ipv4_header);
            break;
        }
        default:
            break;

    }
}
