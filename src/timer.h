//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_TIMER_H
#define YFTCP_TIMER_H

#include "yftcp.h"

#define TIMER_RESOLUTION_CYCLES 1200000000ULL

void
arp_request_timer_cb(__attribute__((unused)) struct rte_timer *tim, void *arg);

#endif //YFTCP_TIMER_H
