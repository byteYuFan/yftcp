//
// Created by wangyufan on 2023-11-20.
//

#include "deal_packet.h"
#include "config.h"
#include "yftcp.h"
#include "ring.h"
#include "arp.h"
#include "ipv4.h"
#include "rarp.h"
#include "udp.h"
#include "udp_server_test.h"

int hz_pkt_process(__attribute__((unused)) void *arg) {
    struct rte_mempool *memory_buffer_pool = (struct rte_mempool *) arg;
    unsigned lcore_id = rte_lcore_id();
    lcore_id = rte_get_next_lcore(lcore_id, 1, 0);
    rte_eal_remote_launch(udp_server_enter, memory_buffer_pool, lcore_id);
    lcore_id = rte_get_next_lcore(lcore_id, 1, 0);
    rte_eal_remote_launch(udp_server_enter_2, memory_buffer_pool, lcore_id);
    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
    while (1) {
        struct rte_mbuf *rx[BURST_SIZE];
        uint16_t num_received = rte_ring_mc_dequeue_burst(ring->in, (void **) rx, BURST_SIZE, NULL);
        unsigned i = 0;
        for (i = 0; i < num_received; i++) {
            struct rte_ether_hdr *ether_header = rte_pktmbuf_mtod(rx[i], struct rte_ether_hdr*);
            if (ether_header->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_ARP)) {
                //获取arp头
                struct rte_arp_hdr *arp_header = rte_pktmbuf_mtod_offset(rx[i],
                                                                         struct rte_arp_hdr*,
                                                                         sizeof(struct rte_ether_hdr));
                hz_process_with_arp(memory_buffer_pool, arp_header);
                rte_pktmbuf_free(rx[i]);
            } else if (ether_header->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_RARP)) {
                struct rte_arp_hdr *arp_header = rte_pktmbuf_mtod_offset(rx[i],
                                                                         struct rte_arp_hdr*,
                                                                         sizeof(struct rte_ether_hdr));
                hz_process_with_reserve_arp(memory_buffer_pool, arp_header);
            } else if (ether_header->ether_type == rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                struct rte_ipv4_hdr *ipv4_header = rte_pktmbuf_mtod_offset(rx[i], struct rte_ipv4_hdr*,
                                                                           sizeof(struct rte_ether_hdr));
                hz_process_with_ipv4(memory_buffer_pool, ether_header, ipv4_header);
                rte_pktmbuf_free(rx[i]);
            } else {
                rte_pktmbuf_free(rx[i]);
                continue;
            }
        }
        hz_process_out_udp(memory_buffer_pool);

    }

    return 0;
}