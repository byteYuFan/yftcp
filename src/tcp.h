//
// Created by wangyufan on 2023-11-21.
//

#ifndef YFTCP_TCP_H
#define YFTCP_TCP_H
#include "yftcp.h"


void hz_process_tcp(struct rte_mempool *memory_buffer_pool, struct rte_ipv4_hdr *ipv4_header);

#endif //YFTCP_TCP_H
