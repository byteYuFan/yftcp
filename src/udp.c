//
// Created by wangyufan on 2023-11-20.
//

#include "udp.h"
#include "config.h"
#include "deal_udp.h"
#include "ring.h"
#include "red_black.h"
#include "conn_status.h"
#include "udp_info.h"
#include "arp_table.h"
#include "arp.h"
#include "socket.h"
#include <unistd.h>
#include "list.h"
#include "function.h"

extern struct rte_ether_addr global_ethernet_device_mac;
extern unsigned int global_ethernet_device_id;
extern struct rte_mempool *global_memory_buffer_pool;

void hz_process_udp(__attribute__((unused))struct rte_mempool *memory_buffer_pool,
                    __attribute__((unused))struct rte_ether_hdr *ether_header,
                    struct rte_ipv4_hdr *ipv4_header) {
    struct rte_udp_hdr *udp_header = (struct rte_udp_hdr *) (ipv4_header + 1);
#if UDP_DEBUG
    printf("src:%s:%d\n", hz_print_ipv4_address(ipv4_header->src_addr), ntohs(udp_header->src_port));
    printf("dst:%s:%d\n", hz_print_ipv4_address(ipv4_header->dst_addr), ntohs(udp_header->dst_port));
    printf("data:%s\n", (char *) (udp_header + 1));

//    uint16_t length = ntohs(udp_header->dgram_len);
//    *((char *) udp_header + length) = '\0';
//    struct rte_mbuf *udp_buffer = hz_send_udp(memory_buffer_pool, &ether_header->d_addr, &ether_header->s_addr,
//                                              ipv4_header->dst_addr,
//                                              ipv4_header->src_addr,
//                                              udp_header->dst_port, udp_header->src_port, (uint8_t *) (udp_header + 1),
//                                              length - sizeof(struct rte_udp_hdr));
//    struct hz_ring_interface *ring = hz_get_ring_interface_instance();
//    rte_ring_mp_enqueue_burst(ring->out, (void **) &udp_buffer, 1, NULL);
//    rte_eth_tx_burst(global_ethernet_device_id, 0, &udp_buffer, 1);
#endif


#if UDP_DEBUG
    if (hz_get_conn_info_by_fd(1) == NULL) {
        struct hz_connection_info *udp_conn_info = hz_init_connection_info(1, IPPROTO_UDP, ipv4_header->src_addr,
                                                                           ipv4_header->dst_addr,
                                                                           udp_header->src_port, udp_header->dst_port,
                                                                           ether_header->s_addr, ether_header->d_addr);

        INSERT_NODE_HEAD(hz_get_conn_list_info_instance()->head, udp_conn_info);
    }

#endif


    struct hz_connection_info *conn_info = hz_get_conn_info_by_socket(ipv4_header->dst_addr, ipv4_header->src_addr,
                                                                      udp_header->dst_port, udp_header->src_port,
                                                                      HZ_PROTOCOL_UDP);
    if (conn_info == NULL)return;
    struct hz_udp_payload *udp_payload = hz_create_udp_payload(ipv4_header->src_addr, ipv4_header->dst_addr,
                                                               udp_header->src_port, udp_header->dst_port,
                                                               HZ_PROTOCOL_UDP, (unsigned char *) (udp_header + 1),
                                                               ntohs(udp_header->dgram_len) -
                                                               sizeof(struct rte_udp_hdr));


    if (!udp_payload)return;
    if (conn_info->receive_buffer) {
        rte_ring_mp_enqueue(conn_info->receive_buffer, udp_payload);
    }
    pthread_mutex_lock(&conn_info->mutex);
    pthread_cond_signal(&conn_info->cond);
    pthread_mutex_unlock(&conn_info->mutex);

}


void hz_process_out_udp(__attribute__((unused))struct rte_mempool *memory_buffer_pool) {
    struct hz_list_connection_info *conn_info = hz_get_conn_list_info_instance();
    struct hz_connection_info *info = NULL;
    struct hz_udp_payload *udp_payload = NULL;
    for (info = conn_info->head; info != NULL; info = info->next) {
        if (info->send_buffer == NULL)return;
        int num_send = rte_ring_mc_dequeue(info->send_buffer, (void **) &udp_payload);
        if (num_send < 0)return;
        if (!udp_payload)return;
        struct rte_ether_addr *dst_mac = hz_get_mac_by_ip(udp_payload->dst_ip);
        struct hz_ring_interface *ring = hz_get_ring_interface_instance();
        if (dst_mac == NULL) {
            struct rte_mbuf *arp_buffer = hz_send_arp(global_memory_buffer_pool, RTE_ARP_OP_REPLY,
                                                      &global_ethernet_device_mac, udp_payload->src_ip,
                                                      udp_payload->dst_ip);

            rte_ring_mp_enqueue_burst(ring->out, (void **) &arp_buffer, 1, NULL);
            rte_ring_mp_enqueue(info->send_buffer, udp_payload);

        } else {
            struct rte_mbuf *udp_buffer = hz_send_udp(global_memory_buffer_pool, &global_ethernet_device_mac, dst_mac,
                                                      udp_payload->src_ip, udp_payload->dst_ip,
                                                      udp_payload->src_port,
                                                      udp_payload->dst_port, udp_payload->data,
                                                      udp_payload->data_length);
            rte_ring_enqueue_burst(ring->out, (void **) &udp_buffer, 1, NULL);
        }

    }

}