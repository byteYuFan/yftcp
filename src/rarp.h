//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_RARP_H
#define YFTCP_RARP_H
#include "yftcp.h"
void hz_process_with_reserve_arp(struct rte_mempool *memory_buffer_pool, struct rte_arp_hdr *arp_header);
#endif //YFTCP_RARP_H
