//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_CONFIG_H
#define YFTCP_CONFIG_H

#define NUMBER_OF_ELEMENTS_INT_THE_MEMORY_BUFFER_POOL (4096-1)
#define BURST_SIZE 32
#define MAKE_IPV4_ADDR(a, b, c, d)(a+(b<<8)+(c<<16)+(d<<24))
#define DEBUG 1
#endif //YFTCP_CONFIG_H
