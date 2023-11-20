//
// Created by wangyufan on 2023-11-20.
//

#include "arp.h"
#include "ring.h"
#include "arp_table.h"
#include "list.h"
#include "function.h"
#include "config.h"

extern uint32_t global_local_ipv4;
extern struct rte_ether_addr global_ethernet_device_mac;
extern struct rte_ether_addr global_default_arp_mac;
extern unsigned int global_ethernet_device_id;


void hz_process_with_arp(struct rte_mempool *memory_buffer_pool, struct rte_arp_hdr *arp_header) {
    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
    if (arp_header->arp_data.arp_tip == global_local_ipv4) {
        if (arp_header->arp_opcode == rte_cpu_to_be_16(RTE_ARP_OP_REQUEST)) {
            struct rte_mbuf *arp_buffer = hz_send_arp(memory_buffer_pool, RTE_ARP_OP_REPLY,
                                                      &arp_header->arp_data.arp_sha,
                                                      arp_header->arp_data.arp_tip, arp_header->arp_data.arp_sip);
            rte_ring_mp_enqueue_burst(ring->out, (void **) &arp_buffer, 1, NULL);
        }
        else if (arp_header->arp_opcode == rte_cpu_to_be_16(RTE_ARP_OP_REPLY)) {
            //如果本机收到了 ARP 回复（RTE_ARP_OP_REPLY），这意味着有另一台设备回应了本机先前发送的 ARP 请求。
            //将mac地址添加到arp table中去
            struct hz_arp_table *arp_table = hz_get_arp_table_instance();
            struct rte_ether_addr *target_mac = hz_get_mac_by_ip(arp_header->arp_data.arp_sip);
            if (target_mac == NULL) {
                // 没有这条记录，记录
                struct hz_arp_table_node *target_info_arp_node = hz_create_arp_table_node(arp_header->arp_data.arp_sip,
                                                                                          arp_header->arp_data.arp_sha,
                                                                                          ARP_NODE_TYPE_DYNAMIC);
                if (target_info_arp_node) {
                    INSERT_NODE_HEAD(arp_table->head, target_info_arp_node);
                    arp_table->count++;
                }

            }
#if DEBUG
            for (struct hz_arp_table_node *iter = arp_table->head; iter != NULL; iter = iter->next) {

                struct in_addr addr;
                addr.s_addr = iter->ip;

                hz_print_ether_addr("arp table --> mac: ", &iter->mac);
                printf(" ip: %s \n", inet_ntoa(addr));
            }
#endif
        }

    }

}

void
hz_build_arp_request(uint8_t *msg, uint16_t option_code, struct rte_ether_addr *dst_mac, uint32_t sip,
                     uint32_t dip) {
    struct rte_ether_hdr *ether_hearer = (struct rte_ether_hdr *) msg;
    rte_memcpy(ether_hearer->s_addr.addr_bytes, global_ethernet_device_mac.addr_bytes, RTE_ETHER_ADDR_LEN);
    if (strncmp((char *) dst_mac->addr_bytes, (char *) (global_default_arp_mac.addr_bytes), RTE_ETHER_ADDR_LEN) !=
        0) {
        struct rte_ether_addr mac = {{0x0}};
        rte_memcpy(ether_hearer->d_addr.addr_bytes, mac.addr_bytes, RTE_ETHER_ADDR_LEN);
    } else {
        rte_memcpy(ether_hearer->d_addr.addr_bytes, dst_mac->addr_bytes, RTE_ETHER_ADDR_LEN);
    }
    ether_hearer->ether_type = htons(RTE_ETHER_TYPE_ARP);
    // ARP
    struct rte_arp_hdr *arp_header = (struct rte_arp_hdr *) (ether_hearer + 1);
    arp_header->arp_hardware = htons(1);
    arp_header->arp_protocol = htons(RTE_ETHER_TYPE_IPV4);
    arp_header->arp_hlen = RTE_ETHER_ADDR_LEN;
    arp_header->arp_plen = sizeof(uint32_t);
    arp_header->arp_opcode = htons(option_code);
    rte_memcpy(arp_header->arp_data.arp_sha.addr_bytes, global_ethernet_device_mac.addr_bytes, RTE_ETHER_ADDR_LEN);
    rte_memcpy(arp_header->arp_data.arp_tha.addr_bytes, dst_mac->addr_bytes, RTE_ETHER_ADDR_LEN);

    arp_header->arp_data.arp_sip = sip;
    arp_header->arp_data.arp_tip = dip;


}

struct rte_mbuf *
hz_send_arp(struct rte_mempool *memory_buffer_pool, uint16_t opcode, struct rte_ether_addr *dst_mac, uint32_t sip,
            uint32_t dip) {
    const unsigned total_length = sizeof(struct rte_ether_hdr) + sizeof(struct rte_arp_hdr);
    struct rte_mbuf *memory_buffer = rte_pktmbuf_alloc(memory_buffer_pool);
    if (!memory_buffer) {
        rte_exit(EXIT_FAILURE, "hz_send_arp:rte_pktmbuf_alloc.\n ");
    }
    memory_buffer->pkt_len = total_length;
    memory_buffer->data_len = total_length;
    uint8_t *pkt_data = rte_pktmbuf_mtod(memory_buffer, uint8_t*);
    hz_build_arp_request(pkt_data, opcode, dst_mac, sip, dip);

    return memory_buffer;
}