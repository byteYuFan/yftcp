//
// Created by wangyufan on 2023-11-20.
//

#include "timer.h"
#include "ring.h"
#include "arp_table.h"
#include "config.h"

extern int32_t global_local_ipv4;
extern struct rte_ether_addr global_ethernet_device_mac;
extern struct rte_ether_addr global_default_arp_mac;

void
arp_request_timer_cb(__attribute__((unused)) struct rte_timer *tim, void *arg) {

    struct rte_mempool *member_buffer_pool = (struct rte_mempool *) arg;
    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
    int i = 0;
    for (i = 1; i <= 254; i++) {
        uint32_t dst_ip = (global_local_ipv4 & 0x00ffffff) | (0xff000000 & (i << 24));
//        uint32_t dst_ip = MAKE_IPV4_ADDR(192, 168, 182, 125);
        struct rte_ether_addr *dst_mac = hz_get_mac_by_ip(dst_ip);
        struct rte_mbuf *arp_buffer = NULL;
        if (dst_mac == NULL) {
            arp_buffer = hz_send_arp(member_buffer_pool, RTE_ARP_OP_REQUEST, &global_default_arp_mac,
                                     global_local_ipv4, dst_ip);

        } else {
            arp_buffer = hz_send_arp(member_buffer_pool, RTE_ARP_OP_REQUEST, dst_mac, global_local_ipv4, dst_ip);
        }
        rte_ring_mp_enqueue_burst(ring->out, (void **) &arp_buffer, 1, NULL);
        rte_pktmbuf_free(arp_buffer);
    }
}
