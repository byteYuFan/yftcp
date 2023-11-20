//
// Created by wangyufan on 2023-11-20.
//

#include "yftcp.h"
#include "ring.h"
#include "arp_table.h"

void hz_process_with_reserve_arp(struct rte_mempool *memory_buffer_pool, struct rte_arp_hdr *arp_header) {
    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
    if (arp_header->arp_opcode == rte_cpu_to_be_16(RTE_ARP_OP_REVREQUEST)) {
        // 1. 查找目标 MAC 地址对应的 IP 地址
        struct rte_ether_addr target_mac = arp_header->arp_data.arp_sha;
        uint32_t target_ip = hz_get_ip_by_mac(&target_mac);

        if (target_ip != 0) {

            struct rte_mbuf *reserve_arp_buffer = hz_send_arp(memory_buffer_pool, RTE_ARP_OP_REVREPLY,
                                                              &target_mac, target_ip, arp_header->arp_data.arp_sip);
            rte_ring_mp_enqueue_burst(ring->out, (void **) &reserve_arp_buffer, 1, NULL);
        }
    }
    if (arp_header->arp_opcode == rte_cpu_to_be_16(RTE_ARP_OP_REVREPLY)) {
        hz_update_arp_table(arp_header->arp_data.arp_sip, arp_header->arp_data.arp_sha);
    }
}