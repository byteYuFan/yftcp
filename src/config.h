//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_CONFIG_H
#define YFTCP_CONFIG_H

#define NUMBER_OF_ELEMENTS_INT_THE_MEMORY_BUFFER_POOL (8046-1)
#define BURST_SIZE 128
#define MAKE_IPV4_ADDR(a, b, c, d)(a+(b<<8)+(c<<16)+(d<<24))
#define DEBUG 0
#define ICMP_DEBUG 0
#define UDP_DEBUG 0
#define UDP_SERVER_TEST 1


#define HZ_PROTOCOL_TCP 1
#define HZ_PROTOCOL_UDP 2

#endif //YFTCP_CONFIG_H
