//
// Created by wangyufan on 2023-11-20.
//

#ifndef YFTCP_ARP_TABLE_H
#define YFTCP_ARP_TABLE_H

#include "yftcp.h"


#define ARP_NODE_TYPE_DYNAMIC 0
#define ARP_NODE_TYPE_STATIC 1

struct hz_arp_table_node {
    uint32_t ip;
    struct rte_ether_addr mac;
    uint8_t type;
    struct hz_arp_table_node *next;
    struct hz_arp_table_node *prev;
};
struct hz_arp_table {
    struct hz_arp_table_node *head;
    unsigned int count;
};

extern struct hz_arp_table *arp_table_singleton_instance;

struct hz_arp_table *hz_get_arp_table_instance(void);

struct rte_ether_addr *hz_get_mac_by_ip(uint32_t ip);

uint32_t hz_get_ip_by_mac(struct rte_ether_addr *mac);

struct hz_arp_table_node *hz_create_arp_table_node(uint32_t ip, struct rte_ether_addr mac, uint8_t type);

void hz_update_arp_table(uint32_t ip, struct rte_ether_addr mac);

#endif //YFTCP_ARP_TABLE_H
