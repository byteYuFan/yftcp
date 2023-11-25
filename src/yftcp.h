//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_YFTCP_H
#define YFTCP_YFTCP_H

#define  WINDOWS 1

#if WINDOWS

#include <test/rte_eal.h>
#include <test/rte_ethdev.h>
#include <test/rte_mbuf.h>
#include <stdio.h>
#include <test/rte_byteorder.h>
#include <arpa/inet.h>
#include <test/rte_timer.h>
#include <test/rte_eal.h>
#include <test/rte_cycles.h>
#include <test/rte_malloc.h>
#include <test/rte_ring.h>
#include <test/rte_byteorder.h>
#include <test/rte_launch.h>
#include <test/rte_eal.h>
#include <test/rte_per_lcore.h>
#include <test/rte_lcore.h>
#include <test/rte_debug.h>



#else
#include <rte_ethdev.h>
#include <rte_mbuf.h>
#include <stdio.h>
#include <rte_byteorder.h>
#include <arpa/inet.h>
#include <rte_common.h>
#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_cycles.h>
#include <rte_timer.h>
#include <rte_debug.h>
#include <rte_malloc.h>
#include <rte_ring.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_per_lcore.h>
#include <rte_lcore.h>
#include <rte_debug.h>
#include <rte_byteorder.h>

#endif

#include "arp.h"
#include "string.h"




#endif //YFTCP_YFTCP_H
