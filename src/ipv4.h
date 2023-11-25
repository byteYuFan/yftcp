//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_IPV4_H
#define YFTCP_IPV4_H


void
hz_process_with_ipv4(struct rte_mempool *memory_buffer_pool, struct rte_ether_hdr *ether_header,
                     struct rte_ipv4_hdr *ipv4_header);

#endif //YFTCP_IPV4_H
