//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_UDP_H
#define YFTCP_UDP_H


#include "yftcp.h"

struct hz_binary_search_tree_node;

void hz_process_udp(struct rte_mempool *memory_buffer_pool, struct rte_ether_hdr *ether_header,
                    struct rte_ipv4_hdr *ipv4_header);

void hz_process_out_udp(struct rte_mempool *memory_buffer_pool);

void deal_out(void *node);

#endif //YFTCP_UDP_H
